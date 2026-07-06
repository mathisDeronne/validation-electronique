// ============================================================================
// FILE: blemanager.cpp
// VERSION: v4 ultra-défensive — logs à chaque étape pour trouver le crash
// ============================================================================

#include "blemanager.hpp"

#include <cstring>
#include <queue>

#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *TAG = "BLE";

// ============================================================================
// État interne
// ============================================================================
static bool     s_initialized  = false;
static bool     s_advertising  = false;
static bool     s_connected    = false;
static uint16_t s_clientCount  = 0;
static uint16_t s_conn_handle  = 0;
static uint16_t s_tx_attr_handle = 0;
static char     s_device_name[32] = {0};
static std::queue<std::string> s_rx_buffer;

bool     blemanager::initialized  = false;
bool     blemanager::advertising  = false;
bool     blemanager::connected    = false;
uint16_t blemanager::clientCount  = 0;

// ============================================================================
// UUIDs — char arrays statiques (pas de std::string, pas de macro)
// ============================================================================
static uint8_t s_svc_uuid_val[16] = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,
                                      0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e};
static uint8_t s_rx_uuid_val[16]  = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,
                                      0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e};
static uint8_t s_tx_uuid_val[16]  = {0x9e,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,
                                      0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,0x6e};

static ble_uuid128_t s_svc_uuid;
static ble_uuid128_t s_rx_uuid;
static ble_uuid128_t s_tx_uuid;

// ============================================================================
// CALLBACK GATT
// ============================================================================
static int onGattAccess(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR)
    {
        uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
        if (len > 0U)
        {
            char buf[256];
            std::memset(buf, 0, sizeof(buf));
            uint16_t copy_len = (len < 255U) ? len : 255U;
            os_mbuf_copydata(ctxt->om, 0, copy_len, buf);
            s_rx_buffer.push(std::string(buf));
            ESP_LOGI(TAG, "RX << %s", buf);
        }
    }

    return 0;
}

// ============================================================================
// TABLE GATT
// ============================================================================
static struct ble_gatt_chr_def s_chars[3];
static struct ble_gatt_svc_def s_svcs[2];

static void buildGattTable()
{
    // UUIDs
    s_svc_uuid.u.type = BLE_UUID_TYPE_128;
    std::memcpy(s_svc_uuid.value, s_svc_uuid_val, 16);

    s_rx_uuid.u.type = BLE_UUID_TYPE_128;
    std::memcpy(s_rx_uuid.value, s_rx_uuid_val, 16);

    s_tx_uuid.u.type = BLE_UUID_TYPE_128;
    std::memcpy(s_tx_uuid.value, s_tx_uuid_val, 16);

    // Caractéristiques
    std::memset(s_chars, 0, sizeof(s_chars));

    s_chars[0].uuid       = &s_rx_uuid.u;
    s_chars[0].access_cb  = onGattAccess;
    s_chars[0].flags      = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP;

    s_chars[1].uuid       = &s_tx_uuid.u;
    s_chars[1].access_cb  = onGattAccess;
    s_chars[1].val_handle = &s_tx_attr_handle;
    s_chars[1].flags      = BLE_GATT_CHR_F_NOTIFY;

    // s_chars[2] = terminateur (déjà à zéro)

    // Service
    std::memset(s_svcs, 0, sizeof(s_svcs));

    s_svcs[0].type            = BLE_GATT_SVC_TYPE_PRIMARY;
    s_svcs[0].uuid            = &s_svc_uuid.u;
    s_svcs[0].characteristics = s_chars;

    // s_svcs[1] = terminateur (déjà à zéro)

    ESP_LOGI(TAG, "GATT table built OK");
}

// ============================================================================
// ADVERTISING
// ============================================================================
static int onGapEvent(struct ble_gap_event *event, void *arg);

static void startAdvertisingInternal()
{
    int rc = 0;

    ESP_LOGI(TAG, "ADV step 1: set fields...");

    struct ble_hs_adv_fields fields;
    std::memset(&fields, 0, sizeof(fields));

    fields.flags            = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name             = (const uint8_t *)s_device_name;
    fields.name_len         = static_cast<uint8_t>(strlen(s_device_name));
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "adv_set_fields FAILED rc=%d", rc);
        return;
    }
    ESP_LOGI(TAG, "ADV step 2: adv_set_fields OK");

    struct ble_gap_adv_params adv_params;
    std::memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                           &adv_params, onGapEvent, NULL);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "adv_start FAILED rc=%d", rc);
        return;
    }

    s_advertising = true;
    ESP_LOGI(TAG, "ADV step 3: Advertising OK -> %s", s_device_name);
}

// ============================================================================
// GAP EVENTS
// ============================================================================
static int onGapEvent(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type)
    {
        case BLE_GAP_EVENT_CONNECT:
        {
            ESP_LOGI(TAG, "GAP connect event, status=%d", event->connect.status);
            if (event->connect.status == 0)
            {
                s_conn_handle = event->connect.conn_handle;
                s_connected   = true;
                s_clientCount = 1;
            }
            else
            {
                s_connected   = false;
                s_clientCount = 0;
                startAdvertisingInternal();
            }
            break;
        }

        case BLE_GAP_EVENT_DISCONNECT:
        {
            ESP_LOGI(TAG, "GAP disconnect");
            s_connected   = false;
            s_clientCount = 0;
            s_conn_handle = 0;
            startAdvertisingInternal();
            break;
        }

        default:
        {
            break;
        }
    }

    return 0;
}

// ============================================================================
// NIMBLE SYNC + TASK
// ============================================================================
static void onNimbleSync()
{
    ESP_LOGI(TAG, "SYNC step 1: ensure addr...");

    int rc = ble_hs_util_ensure_addr(0);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "ensure_addr FAILED rc=%d", rc);
        return;
    }
    ESP_LOGI(TAG, "SYNC step 2: addr OK");

    uint8_t addr[6] = {0};
    uint8_t addr_type = 0;
    rc = ble_hs_id_infer_auto(0, &addr_type);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "id_infer_auto FAILED rc=%d", rc);
        return;
    }

    rc = ble_hs_id_copy_addr(addr_type, addr, NULL);
    ESP_LOGI(TAG, "SYNC step 3: BLE addr=%02x:%02x:%02x:%02x:%02x:%02x type=%d",
             addr[5], addr[4], addr[3], addr[2], addr[1], addr[0], addr_type);

    startAdvertisingInternal();
}

static void onNimbleReset(int reason)
{
    ESP_LOGW(TAG, "NimBLE reset reason=%d", reason);
}

static void nimbleHostTask(void *param)
{
    (void)param;
    ESP_LOGI(TAG, "Host task started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

// ============================================================================
// INTERFACE PUBLIQUE
// ============================================================================

bool blemanager::init(const std::string &deviceName)
{
    bool result = false;

    if (s_initialized)
    {
        return true;
    }

    ESP_LOGI(TAG, "INIT step 1: NVS...");

    // Copier le nom dans un buffer statique (pas de std::string dans les callbacks)
    std::strncpy(s_device_name, deviceName.c_str(), sizeof(s_device_name) - 1U);

    // NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS FAILED %d", ret);
        return false;
    }
    ESP_LOGI(TAG, "INIT step 2: NVS OK");

    // GATT table
    buildGattTable();
    ESP_LOGI(TAG, "INIT step 3: GATT table OK");

    // NimBLE
    ret = nimble_port_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "nimble_port_init FAILED %d", ret);
        return false;
    }
    ESP_LOGI(TAG, "INIT step 4: nimble_port_init OK");

    // Host config
    ble_hs_cfg.sync_cb  = onNimbleSync;
    ble_hs_cfg.reset_cb = onNimbleReset;

    // GAP name
    ble_svc_gap_device_name_set(s_device_name);

    // Services GATT
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ESP_LOGI(TAG, "INIT step 5: gap+gatt init OK");

    int rc = ble_gatts_count_cfg(s_svcs);
    ESP_LOGI(TAG, "INIT step 6: gatts_count_cfg rc=%d", rc);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "gatts_count_cfg FAILED");
        return false;
    }

    rc = ble_gatts_add_svcs(s_svcs);
    ESP_LOGI(TAG, "INIT step 7: gatts_add_svcs rc=%d", rc);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "gatts_add_svcs FAILED");
        return false;
    }

    // Lancer NimBLE
    nimble_port_freertos_init(nimbleHostTask);
    ESP_LOGI(TAG, "INIT step 8: freertos task started");

    s_initialized = true;
    initialized   = true;
    result        = true;

    ESP_LOGI(TAG, "BLE initialise: %s", s_device_name);

    return result;
}

void blemanager::start()
{
    if (s_initialized && !s_advertising)
    {
        startAdvertisingInternal();
        advertising = true;
    }
}

void blemanager::stop()
{
    if (s_advertising)
    {
        ble_gap_adv_stop();
        s_advertising = false;
        advertising   = false;
    }
}

bool blemanager::isConnected()
{
    connected = s_connected;
    return s_connected;
}

uint16_t blemanager::connectedClients()
{
    clientCount = s_clientCount;
    return s_clientCount;
}

bool blemanager::notify(const uint8_t *data, size_t length)
{
    bool result = false;

    if (s_connected && (s_tx_attr_handle != 0U))
    {
        struct os_mbuf *om = ble_hs_mbuf_from_flat(data, static_cast<uint16_t>(length));
        if (om != NULL)
        {
            int rc = ble_gatts_notify_custom(s_conn_handle, s_tx_attr_handle, om);
            result = (rc == 0);
        }
    }

    return result;
}

bool blemanager::notify(const std::string &message)
{
    return notify(reinterpret_cast<const uint8_t *>(message.c_str()),
                  message.length());
}

bool blemanager::available()
{
    return !s_rx_buffer.empty();
}

std::string blemanager::read()
{
    std::string msg = "";

    if (!s_rx_buffer.empty())
    {
        msg = s_rx_buffer.front();
        s_rx_buffer.pop();
    }

    return msg;
}
