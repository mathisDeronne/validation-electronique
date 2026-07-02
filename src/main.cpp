#include <Arduino.h>
#include <NimBLEDevice.h>

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define TX_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

NimBLECharacteristic *txChar;
bool deviceConnected = false;

class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer)
    {
        deviceConnected = true;
        Serial.println("Client connecté");
    }

    void onDisconnect(NimBLEServer *pServer)
    {
        deviceConnected = false;
        Serial.println("Client déconnecté");
        NimBLEDevice::getAdvertising()->start();
    }
};

class RxCallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();

        Serial.print("Reçu BLE: ");
        Serial.println(value.c_str());
    }
};

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("Démarrage BLE NimBLE...");

    NimBLEDevice::init("ESP32-PICO-D4");

    NimBLEServer *server = NimBLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

    NimBLEService *service = server->createService(SERVICE_UUID);

    // TX (ESP32 -> téléphone)
    txChar = service->createCharacteristic(
        TX_UUID,
        NIMBLE_PROPERTY::NOTIFY);

    // RX (téléphone -> ESP32)
    NimBLECharacteristic *rxChar = service->createCharacteristic(
        RX_UUID,
        NIMBLE_PROPERTY::WRITE);

    rxChar->setCallbacks(new RxCallbacks());

    service->start();

    NimBLEAdvertising *advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(SERVICE_UUID);
    advertising->start();

    Serial.println("BLE prêt !");
}

void loop()
{
    if (deviceConnected)
    {
        static int counter = 0;

        String msg = "Hello ESP32 #" + String(counter++);

        txChar->setValue(msg.c_str());
        txChar->notify();

        Serial.println("Envoyé: " + msg);

        delay(2000);
    }
}