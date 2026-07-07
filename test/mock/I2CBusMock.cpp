#include "I2CBusMock.hpp"

void I2CBusMock::setRegister(uint8_t reg, uint16_t value)
{
    _regs[reg] = value;
}

void I2CBusMock::setFailRead(bool v)
{
    _failRead = v;
}

void I2CBusMock::setFailWrite(bool v)
{
    _failWrite = v;
}

I2CStatus I2CBusMock::read16(uint8_t, uint8_t reg, uint16_t &data)
{
    if (_failRead)
        return I2CStatus::ERROR;

    if (_regs.find(reg) == _regs.end())
        return I2CStatus::ERROR;

    data = _regs[reg];
    return I2CStatus::OK;
}

I2CStatus I2CBusMock::write16(uint8_t, uint8_t reg, uint16_t data)
{
    if (_failWrite)
        return I2CStatus::ERROR;

    _regs[reg] = data;
    return I2CStatus::OK;
}