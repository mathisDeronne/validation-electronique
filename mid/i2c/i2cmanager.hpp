#pragma once

#include <cstdint>

#include <esp_err.h>
#include <driver/i2c.h>

class I2CManager
{
public:
    //--------------------------------------------------------------
    // Initialisation
    //--------------------------------------------------------------

    static esp_err_t init();
    static void deinit();

    //--------------------------------------------------------------
    // Diagnostic
    //--------------------------------------------------------------

    static uint8_t scan(bool printResult = true);
    static bool devicePresent(uint8_t address);

    //--------------------------------------------------------------
    // Lecture / écriture
    //--------------------------------------------------------------

    static esp_err_t write(
        uint8_t address,
        const uint8_t *data,
        size_t length);

    static esp_err_t read(
        uint8_t address,
        uint8_t *data,
        size_t length);

    //--------------------------------------------------------------
    // Registres
    //--------------------------------------------------------------

    static esp_err_t writeRegister(
        uint8_t address,
        uint8_t reg,
        uint8_t value);

    static esp_err_t readRegister(
        uint8_t address,
        uint8_t reg,
        uint8_t *value);

private:
    static constexpr i2c_port_t PORT = I2C_NUM_0;
    static constexpr TickType_t TIMEOUT = pdMS_TO_TICKS(100);

    static bool initialized;
};