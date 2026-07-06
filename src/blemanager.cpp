// ============================================================================
// FILE: blemanager.cpp
// PURPOSE: Implémentation BLE avec Bluedroid (ESP-IDF natif, pas NimBLE)
//          Compatible blemanager.hpp SANS MODIFICATION
// ============================================================================

#include "blemanager.hpp"

#include <cstring>
#include <cstdio>
#include <queue>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

static const char *TAG = "BLE";

// ============================================================================
// État interne
// ============================================================================
static bool     s_initialized   = false;
static bool     s_advertising   = false;
static bool     s_connected     = false;
static uint16_t s_clientCount   = 0;
static uint16_t s_conn_id       = 0;
static uint16_t s_gatts_if      = ESP_GATT_IF_NONE;
static uint16_t s_service_handle = 0;
static uint16_t s_tx_handle     = 0;
static uint16_t s_rx_handle     = 0;
static char     s_device_name[32] = {0};
static std::queue<std::string> s_rx_buffer;

// Compteur pour savoir quelle caractéristique on ajoute
static uint8_t s_char_add_step = 0;

// Variables statiques obligatoires (déclarées dans le .hpp)
bool     blemanager::initialized  = false;
bool     blemanager::advertising  = false;
bool     blemanager::connected    = false;
uint16_t blemanager::clientCount  = 0;

// ============================================================================
// UUIDs Nordic UART Service (128 bits, format little-endian)
// ============================================================================
static const uint8_t SERVICE_UUID[16] = {
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e
};

static const uint8_t CHAR_RX_UUID[16] = {
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e
};

static const uint8_t CHAR_TX_UUID[16] = {
    0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
    0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e
};

// ============================================================================
// ADVERTISING DATA
// ============================================================================
static esp_ble_adv_params_t s_adv_params;

static void startAdvertising()
{
    esp_ble_gap_start_advertising(&s_adv_params);
}

// ============================================================================
// CALLBACK GAP (advertising)
// ============================================================================
static void gapEventHandler(esp_gap_ble_cb_event_t event,
                            esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        {
            ESP_LOGI(TAG, "ADV data set complete -> start advertising");
            startAdvertising();
            break;
        }

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        {
            if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS)
            {
                s_advertising = true;
                ESP_LOGI(TAG, "Advertising started: %s", s_device_name);
            }
            else
            {
                ESP_LOGE(TAG, "Advertising start FAILED");
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

// ============================================================================
// CALLBACK GATTS (service, caractéristiques, lecture/écriture)
// ============================================================================
static void gattsEventHandler(esp_gatts_cb_event_t event,
                              esp_gatt_if_t gatts_if,
                              esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
        // App enregistrée → créer le service
        case ESP_GATTS_REG_EVT:
        {
            ESP_LOGI(TAG, "GATTS REG -> create service");
            s_gatts_if = gatts_if;

            // Nom du device
            esp_ble_gap_set_device_name(s_device_name);

            // Advertising data
            esp_ble_adv_data_t adv_data;
            std::memset(&adv_data, 0, sizeof(adv_data));
            adv_data.set_scan_rsp      = false;
            adv_data.include_name      = true;
            adv_data.include_txpower   = false;
            adv_data.flag              = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
            esp_ble_gap_config_adv_data(&adv_data);

            // Créer le service (128 bits UUID)
            esp_gatt_srvc_id_t service_id;
            std::memset(&service_id, 0, sizeof(service_id));
            service_id.is_primary = true;
            service_id.id.inst_id = 0;
            service_id.id.uuid.len = ESP_UUID_LEN_128;
            std::memcpy(service_id.id.uuid.uuid.uuid128, SERVICE_UUID, 16);

            // 4 handles : service + rx_char + tx_char + tx_descr(CCC)
            esp_ble_gatts_create_service(gatts_if, &service_id, 8);
            break;
        }

        // Service créé → ajouter la caractéristique RX
        case ESP_GATTS_CREATE_EVT:
        {
            ESP_LOGI(TAG, "Service created -> add RX char");
            s_service_handle = param->create.service_handle;

            // Démarrer le service
            esp_ble_gatts_start_service(s_service_handle);

            // Ajouter RX (téléphone écrit ici)
            esp_bt_uuid_t rx_uuid;
            rx_uuid.len = ESP_UUID_LEN_128;
            std::memcpy(rx_uuid.uuid.uuid128, CHAR_RX_UUID, 16);

            esp_gatt_perm_t perm = ESP_GATT_PERM_WRITE;
            esp_gatt_char_prop_t prop = ESP_GATT_CHAR_PROP_BIT_WRITE |
                                        ESP_GATT_CHAR_PROP_BIT_WRITE_NR;

            s_char_add_step = 0;
            esp_ble_gatts_add_char(s_service_handle, &rx_uuid, perm, prop,
                                   NULL, NULL);
            break;
        }

        // Caractéristique ajoutée → ajouter la suivante
        case ESP_GATTS_ADD_CHAR_EVT:
        {
            if (s_char_add_step == 0)
            {
                // RX ajouté → sauver le handle, ajouter TX
                s_rx_handle = param->add_char.attr_handle;
                ESP_LOGI(TAG, "RX char added (handle=%d) -> add TX char", s_rx_handle);

                esp_bt_uuid_t tx_uuid;
                tx_uuid.len = ESP_UUID_LEN_128;
                std::memcpy(tx_uuid.uuid.uuid128, CHAR_TX_UUID, 16);

                esp_gatt_perm_t perm = ESP_GATT_PERM_READ;
                esp_gatt_char_prop_t prop = ESP_GATT_CHAR_PROP_BIT_NOTIFY;

                s_char_add_step = 1;
                esp_ble_gatts_add_char(s_service_handle, &tx_uuid, perm, prop,
                                       NULL, NULL);
            }
            else
            {
                // TX ajouté → sauver le handle, ajouter le descripteur CCC
                s_tx_handle = param->add_char.attr_handle;
                ESP_LOGI(TAG, "TX char added (handle=%d) -> add CCC descr", s_tx_handle);

                // Descripteur CCC (Client Characteristic Configuration)
                // Nécessaire pour que le client puisse activer les notifications
                esp_bt_uuid_t ccc_uuid;
                ccc_uuid.len = ESP_UUID_LEN_16;
                ccc_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

                esp_gatt_perm_t perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;

                uint16_t ccc_val = 0x0000;
                esp_attr_value_t ccc_attr;
                std::memset(&ccc_attr, 0, sizeof(ccc_attr));
                ccc_attr.attr_max_len = 2;
                ccc_attr.attr_len     = 2;
                ccc_attr.attr_value   = (uint8_t *)&ccc_val;

                esp_ble_gatts_add_char_descr(s_service_handle, &ccc_uuid,
                                              perm, &ccc_attr, NULL);
            }
            break;
        }

        // Descripteur ajouté → tout est prêt
        case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        {
            ESP_LOGI(TAG, "CCC descriptor added -> BLE ready");
            break;
        }

        // Client connecté
        case ESP_GATTS_CONNECT_EVT:
        {
            s_conn_id     = param->connect.conn_id;
            s_connected   = true;
            s_clientCount = 1;
            ESP_LOGI(TAG, "Client connecte (conn_id=%d)", s_conn_id);
            break;
        }

        // Client déconnecté
        case ESP_GATTS_DISCONNECT_EVT:
        {
            s_connected   = false;
            s_clientCount = 0;
            s_conn_id     = 0;
            ESP_LOGI(TAG, "Client deconnecte -> restart advertising");
            startAdvertising();
            break;
        }

        // Client a écrit sur RX (commande reçue)
        case ESP_GATTS_WRITE_EVT:
        {
            if (param->write.handle == s_rx_handle)
            {
                if (param->write.len > 0)
                {
                    char buf[256];
                    uint16_t len = (param->write.len < 255U) ? param->write.len : 255U;
                    std::memcpy(buf, param->write.value, len);
                    buf[len] = '\0';

                    s_rx_buffer.push(std::string(buf));
                    ESP_LOGI(TAG, "RX << %s", buf);
                }
            }

            // Répondre au client si nécessaire
            if (param->write.need_rsp)
            {
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                             param->write.trans_id,
                                             ESP_GATT_OK, NULL);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

// ============================================================================
// INTERFACE PUBLIQUE blemanager
// ============================================================================

bool blemanager::init(const std::string &deviceName)
{
    if (s_initialized)
    {
        return true;
    }

    // Copier le nom dans un buffer statique
    std::strncpy(s_device_name, deviceName.c_str(), sizeof(s_device_name) - 1U);

    ESP_LOGI(TAG, "Init: %s", s_device_name);

    // NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS failed: %d", ret);
        return false;
    }

    // Libérer la mémoire du BT Classic (on utilise que le BLE)
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    // Contrôleur BT
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "BT controller init failed: %d", ret);
        return false;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "BT controller enable failed: %d", ret);
        return false;
    }

    // Bluedroid
    ret = esp_bluedroid_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Bluedroid init failed: %d", ret);
        return false;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Bluedroid enable failed: %d", ret);
        return false;
    }

    // Callbacks
    esp_ble_gatts_register_callback(gattsEventHandler);
    esp_ble_gap_register_callback(gapEventHandler);

    // Paramètres advertising
    std::memset(&s_adv_params, 0, sizeof(s_adv_params));
    s_adv_params.adv_int_min       = 0x20;
    s_adv_params.adv_int_max       = 0x40;
    s_adv_params.adv_type          = ADV_TYPE_IND;
    s_adv_params.own_addr_type     = BLE_ADDR_TYPE_PUBLIC;
    s_adv_params.channel_map       = ADV_CHNL_ALL;
    s_adv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

    // Enregistrer l'app GATTS (déclenche ESP_GATTS_REG_EVT)
    esp_ble_gatts_app_register(0);

    s_initialized = true;
    initialized   = true;

    ESP_LOGI(TAG, "BLE initialise: %s", s_device_name);

    return true;
}

void blemanager::start()
{
    if (s_initialized && !s_advertising)
    {
        startAdvertising();
        advertising = true;
    }
}

void blemanager::stop()
{
    if (s_advertising)
    {
        esp_ble_gap_stop_advertising();
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

    if (s_connected && (s_tx_handle != 0U) && (s_gatts_if != ESP_GATT_IF_NONE))
    {
        esp_err_t err = esp_ble_gatts_send_indicate(
            s_gatts_if,
            s_conn_id,
            s_tx_handle,
            static_cast<uint16_t>(length),
            const_cast<uint8_t *>(data),
            false  // false = notification (pas d'ACK), true = indication (avec ACK)
        );

        result = (err == ESP_OK);
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
