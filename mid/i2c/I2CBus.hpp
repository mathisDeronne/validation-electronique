#pragma once

#include <stdint.h>
#include "driver/i2c.h"

/**
 * @brief Statut I2C
 */
enum class I2CStatus
{
    OK = 0,
    ERROR,
    NO_ACK,
    INVALID_ARG
};

/**
 * @brief Driver I2C ESP-IDF (bas niveau)
 */
class I2CBus
{
public:
    I2CBus(i2c_port_t port = I2C_NUM_0);

    void begin(int sda, int scl, uint32_t freq_hz);

    bool devicePresent(uint8_t address);

    I2CStatus read16(uint8_t address, uint8_t reg, uint16_t &data);
    I2CStatus write16(uint8_t address, uint8_t reg, uint16_t data);

private:
    i2c_port_t _port;
};