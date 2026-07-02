#pragma once

#include <cstdint>
#include <string>

class BLEManager
{
public:
    //--------------------------------------------------------------
    // Initialisation
    //--------------------------------------------------------------

    static bool init(const std::string &deviceName);

    static void start();

    static void stop();

    //--------------------------------------------------------------
    // Connexion
    //--------------------------------------------------------------

    static bool isConnected();

    static uint16_t connectedClients();

    //--------------------------------------------------------------
    // Envoi de données
    //--------------------------------------------------------------

    static bool notify(const uint8_t *data,
                       size_t length);

    static bool notify(const std::string &message);

    //--------------------------------------------------------------
    // Réception
    //--------------------------------------------------------------

    static bool available();

    static std::string read();

private:
    static bool initialized;
};