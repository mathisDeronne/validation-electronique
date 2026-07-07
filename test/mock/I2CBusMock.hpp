#pragma once

#include <cstdint>
#include <unordered_map>
#include "INA237.hpp" // pour I2CStatus

/**
 * @brief Mock du bus I2C pour tests unitaires
 */
class I2CBusMock
{
public:
    I2CStatus read16(uint8_t addr, uint8_t reg, uint16_t &data);
    I2CStatus write16(uint8_t addr, uint8_t reg, uint16_t data);

    void setRegister(uint8_t reg, uint16_t value);

    void setFailRead(bool v);
    void setFailWrite(bool v);

private:
    std::unordered_map<uint8_t, uint16_t> _regs;

    bool _failRead = false;
    bool _failWrite = false;
};