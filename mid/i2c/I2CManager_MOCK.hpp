#pragma once

#include <cstdint>
#include <stdio.h>

class I2CManager
{
public:
    //--------------------------------------------------
    // INIT
    //--------------------------------------------------
    static bool init();
    static void deinit();

    //--------------------------------------------------
    // DIAG
    //--------------------------------------------------
    static uint8_t scan(bool printResult = true);
    static bool devicePresent(uint8_t address);

    //--------------------------------------------------
    // READ / WRITE
    //--------------------------------------------------
    static bool write(uint8_t address,
                      const uint8_t *data,
                      size_t length);

    static bool read(uint8_t address,
                     uint8_t *data,
                     size_t length);

    //--------------------------------------------------
    // REGISTERS
    //--------------------------------------------------
    static bool writeRegister(uint8_t address,
                              uint8_t reg,
                              uint8_t value);

    static bool readRegister(uint8_t address,
                             uint8_t reg,
                             uint8_t *value);

private:
    static bool initialized;
};