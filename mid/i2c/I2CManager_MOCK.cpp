#include "I2CManager_MOCK.hpp"

bool I2CManager::initialized = false;

// --------------------------------------------------
// INIT
// --------------------------------------------------
bool I2CManager::init()
{
    printf("[MOCK] I2C init\n");
    initialized = true;
    return true;
}

void I2CManager::deinit()
{
    printf("[MOCK] I2C deinit\n");
    initialized = false;
}

// --------------------------------------------------
// SCAN (valeurs fake)
// --------------------------------------------------
uint8_t I2CManager::scan(bool printResult)
{
    if (!initialized)
        return 0;

    if (printResult)
    {
        printf("[MOCK] Device found at 0x40\n");
        printf("[MOCK] Device found at 0x48\n");
    }

    return 2;
}

// --------------------------------------------------
// DEVICE PRESENT
// --------------------------------------------------
bool I2CManager::devicePresent(uint8_t address)
{
    if (!initialized)
        return false;

    return (address == 0x40 || address == 0x48);
}

// --------------------------------------------------
// WRITE (always OK)
// --------------------------------------------------
bool I2CManager::write(uint8_t address,
                       const uint8_t *data,
                       size_t length)
{
    if (!initialized)
        return false;

    printf("[MOCK] write to 0x%02X (%zu bytes)\n", address, length);
    return true;
}

// --------------------------------------------------
// READ (fake data)
// --------------------------------------------------
bool I2CManager::read(uint8_t address,
                      uint8_t *data,
                      size_t length)
{
    if (!initialized)
        return false;

    for (size_t i = 0; i < length; i++)
        data[i] = 0xAB;

    printf("[MOCK] read from 0x%02X (%zu bytes)\n", address, length);
    return true;
}

// --------------------------------------------------
// REGISTER WRITE
// --------------------------------------------------
bool I2CManager::writeRegister(uint8_t address,
                               uint8_t reg,
                               uint8_t value)
{
    if (!initialized)
        return false;

    printf("[MOCK] write reg 0x%02X = 0x%02X on 0x%02X\n",
           reg, value, address);

    return true;
}

// --------------------------------------------------
// REGISTER READ
// --------------------------------------------------
bool I2CManager::readRegister(uint8_t address,
                              uint8_t reg,
                              uint8_t *value)
{
    if (!initialized)
        return false;

    *value = 0x55;

    printf("[MOCK] read reg 0x%02X from 0x%02X = 0x55\n",
           reg, address);

    return true;
}