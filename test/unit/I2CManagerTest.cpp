#include <stdio.h>

#include "I2CManager_MOCK.hpp"
#include "I2CDevicesConfig.hpp"

int main()
{
    printf("\n=============================\n");
    printf("   I2C MANAGER UNIT TEST (MOCK)\n");
    printf("=============================\n\n");

    //--------------------------------------------------
    // INIT
    //--------------------------------------------------
    printf("[TEST] init I2C...\n");

    if (!I2CManager::init())
    {
        printf("[FAIL] init\n");
        return -1;
    }

    printf("[OK] init success\n");

    //--------------------------------------------------
    // SCAN
    //--------------------------------------------------
    printf("\n[TEST] scan...\n");

    uint8_t count = I2CManager::scan(true);
    printf("[INFO] devices = %d\n", count);

    //--------------------------------------------------
    // DEVICE CHECK
    //--------------------------------------------------
    printf("\n[TEST] device check...\n");

    if (I2CManager::devicePresent(I2CDevices::Power::INA237_ADRESS))
        printf("[OK] INA237 detected\n");
    else
        printf("[WARN] not detected\n");

    //--------------------------------------------------
    // WRITE
    //--------------------------------------------------
    printf("\n[TEST] write register...\n");

    I2CManager::writeRegister(0x40, 0x10, 0xAA);

    //--------------------------------------------------
    // READ
    //--------------------------------------------------
    printf("\n[TEST] read register...\n");

    uint8_t value = 0;
    I2CManager::readRegister(0x40, 0x10, &value);

    printf("[INFO] value = 0x%02X\n", value);

    //--------------------------------------------------
    // END
    //--------------------------------------------------
    printf("\n=============================\n");
    printf("   TEST FINISHED\n");
    printf("=============================\n");

    return 0;
}