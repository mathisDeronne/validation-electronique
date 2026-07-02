#include <stdio.h>
#include <string>

#include "blemanager.hpp"

int main()
{
    printf("\n=============================\n");
    printf("   blemanager UNIT TEST\n");
    printf("=============================\n\n");

    //----------------------------------------------------------
    // INIT
    //----------------------------------------------------------
    printf("[TEST] init BLE...\n");

    bool ok = blemanager::init("VentecMonitoring");

    if (ok)
        printf("[OK] init success\n");
    else
    {
        printf("[FAIL] init failed\n");
        return -1;
    }

    //----------------------------------------------------------
    // START ADVERTISING
    //----------------------------------------------------------
    printf("\n[TEST] start advertising...\n");

    blemanager::start();

    if (blemanager::isConnected())
        printf("[INFO] already connected (unexpected in mock)\n");
    else
        printf("[OK] not connected (normal)\n");

    //----------------------------------------------------------
    // CONNECTION SIMULATION
    //----------------------------------------------------------
    printf("\n[SIM] fake connection...\n");

    // simulation simple (mock internal state)
    // dans ton vrai BLE ça viendra des callbacks
    // ici on force pour tester la logique

    // ⚠️ hack propre pour test (on n’expose pas en prod)
    extern bool blemanager_connected;
    extern uint16_t blemanager_clientCount;

    //----------------------------------------------------------
    // SEND TEST (will fail if not connected)
    //----------------------------------------------------------
    printf("\n[TEST] notify before connection...\n");

    if (!blemanager::notify("Hello"))
        printf("[OK] notify blocked (no client)\n");
    else
        printf("[WARN] notify accepted unexpectedly\n");

    //----------------------------------------------------------
    // RECEPTION TEST
    //----------------------------------------------------------
    printf("\n[TEST] RX check...\n");

    if (!blemanager::available())
        printf("[OK] no message (expected)\n");

    std::string msg = blemanager::read();

    if (msg.empty())
        printf("[OK] read empty\n");

    //----------------------------------------------------------
    // STOP
    //----------------------------------------------------------
    printf("\n[TEST] stop BLE...\n");

    blemanager::stop();

    printf("[OK] BLE stopped\n");

    //----------------------------------------------------------
    // END
    //----------------------------------------------------------
    printf("\n=============================\n");
    printf("   TEST FINISHED\n");
    printf("=============================\n");

    return 0;
}