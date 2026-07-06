#include "blemanager.hpp"

#include <queue>

//==============================================================
// Variables statiques
//==============================================================
bool blemanager::initialized = false;
bool blemanager::advertising = false;
bool blemanager::connected = false;
uint16_t blemanager::clientCount = 0;

// buffer RX simulé
static std::queue<std::string> rxBuffer;

//==============================================================
// INIT
//==============================================================
bool blemanager::init(const std::string &deviceName)
{
    (void)deviceName;

    initialized = true;
    advertising = false;
    connected = false;
    clientCount = 0;

    return true;
}

//==============================================================
// START / STOP
//==============================================================
void blemanager::start()
{
    if (!initialized)
        return;

    advertising = true;
}

void blemanager::stop()
{
    advertising = false;
    connected = false;
    clientCount = 0;
}

//==============================================================
// STATE
//==============================================================
bool blemanager::isConnected()
{
    return connected;
}

uint16_t blemanager::connectedClients()
{
    return clientCount;
}

//==============================================================
// NOTIFY
//==============================================================
bool blemanager::notify(const uint8_t *data, size_t length)
{
    if (!connected)
        return false;

    (void)data;
    (void)length;

    // MOCK : on accepte toujours
    return true;
}

bool blemanager::notify(const std::string &message)
{
    if (!connected)
        return false;

    (void)message;

    return true;
}

//==============================================================
// RECEPTION
//==============================================================
bool blemanager::available()
{
    return !rxBuffer.empty();
}

std::string blemanager::read()
{
    if (rxBuffer.empty())
        return "";

    std::string msg = rxBuffer.front();
    rxBuffer.pop();

    return msg;
}