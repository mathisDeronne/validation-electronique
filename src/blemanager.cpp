// ============================================================================
// FILE: blemanager.cpp
// PURPOSE: Implémentation BLE avec la librairie Arduino ESP32 BLE
//          Compatible blemanager.hpp SANS MODIFICATION
// ============================================================================

#include "blemanager.hpp"

#include <cstring>
#include <queue>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ============================================================================
// UUIDs Nordic UART Service (compatibles nRF Connect)
// ============================================================================
#define SERVICE_UUID    "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHAR_RX_UUID    "6e400002-b5a3-f393-e0a9-e50e24dcca9e"  // ESP32 reçoit
#define CHAR_TX_UUID    "6e400003-b5a3-f393-e0a9-e50e24dcca9e"  // ESP32 envoie

// ============================================================================
// État interne
// ============================================================================
static BLEServer         *s_server  = nullptr;
static BLECharacteristic *s_txChar  = nullptr;
static BLECharacteristic *s_rxChar  = nullptr;
static bool               s_connected   = false;
static uint16_t           s_clientCount = 0;
static std::queue<std::string> s_rx_buffer;

// Variables statiques obligatoires (déclarées dans le .hpp)
bool     blemanager::initialized  = false;
bool     blemanager::advertising  = false;
bool     blemanager::connected    = false;
uint16_t blemanager::clientCount  = 0;

// ============================================================================
// CALLBACKS SERVEUR (connexion / déconnexion)
// ============================================================================
class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer) override
    {
        (void)pServer;
        s_connected   = true;
        s_clientCount = 1;
        printf("[BLE] Client connecte\n");
    }

    void onDisconnect(BLEServer *pServer) override
    {
        (void)pServer;
        s_connected   = false;
        s_clientCount = 0;
        printf("[BLE] Client deconnecte\n");

        // Relancer l'advertising pour qu'un nouveau client puisse se connecter
        BLEDevice::startAdvertising();
    }
};

// ============================================================================
// CALLBACKS CARACTÉRISTIQUE RX (quand le téléphone envoie une commande)
// ============================================================================
class RxCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic) override
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0)
        {
            s_rx_buffer.push(value);
            printf("[BLE] RX << %s\n", value.c_str());
        }
    }
};

// ============================================================================
// INTERFACE PUBLIQUE blemanager
// ============================================================================

bool blemanager::init(const std::string &deviceName)
{
    if (initialized)
    {
        return true;
    }

    printf("[BLE] Init: %s\n", deviceName.c_str());

    // Initialiser le device BLE
    BLEDevice::init(deviceName);

    // Créer le serveur
    s_server = BLEDevice::createServer();
    if (s_server == nullptr)
    {
        printf("[BLE] ERREUR: createServer failed\n");
        return false;
    }
    s_server->setCallbacks(new ServerCallbacks());

    // Créer le service UART
    BLEService *service = s_server->createService(SERVICE_UUID);
    if (service == nullptr)
    {
        printf("[BLE] ERREUR: createService failed\n");
        return false;
    }

    // Caractéristique TX (ESP32 → téléphone, notification)
    s_txChar = service->createCharacteristic(
        CHAR_TX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    s_txChar->addDescriptor(new BLE2902());

    // Caractéristique RX (téléphone → ESP32, écriture)
    s_rxChar = service->createCharacteristic(
        CHAR_RX_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
    );
    s_rxChar->setCallbacks(new RxCallbacks());

    // Démarrer le service
    service->start();

    // Configurer l'advertising
    BLEAdvertising *adv = BLEDevice::getAdvertising();
    adv->addServiceUUID(SERVICE_UUID);
    adv->setScanResponse(true);
    adv->setMinPreferred(0x06);
    adv->setMinPreferred(0x12);

    initialized = true;
    printf("[BLE] Init OK\n");

    return true;
}

void blemanager::start()
{
    if (initialized && !advertising)
    {
        BLEDevice::startAdvertising();
        advertising = true;
        printf("[BLE] Advertising started\n");
    }
}

void blemanager::stop()
{
    if (advertising)
    {
        BLEDevice::getAdvertising()->stop();
        advertising = false;
        printf("[BLE] Advertising stopped\n");
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

    if (s_connected && (s_txChar != nullptr))
    {
        s_txChar->setValue(const_cast<uint8_t *>(data), length);
        s_txChar->notify();
        result = true;
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
