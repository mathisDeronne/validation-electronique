#include "INA237.hpp"
#include <cstdio>

INA237::INA237(I2CBus &bus,
               uint8_t address)
    : _bus(bus),
      _address(address)
{
}

bool INA237::begin()
{
    return isConnected();
}

bool INA237::isConnected()
{
    uint16_t id = 0;

    if (_bus.read16(
            _address,
            static_cast<uint8_t>(INA237Defs::Register::MANUFACTURER_ID),
            id) != I2CStatus::OK)
    {
        return false;
    }

    printf("[INA237] MANUFACTURER_ID RAW = 0x%04X\n", id);

    // ✔ comparaison correcte
    if (id == INA237Defs::MANUFACTURER_ID_VALUE)
    {
        return true;
    }

    // 🔧 fallback endian swap (sécurité ESP32)
    uint16_t swapped = (id >> 8) | (id << 8);

    printf("[INA237] MANUFACTURER_ID SWAPPED = 0x%04X\n", swapped);

    return swapped == INA237Defs::MANUFACTURER_ID_VALUE;
}

bool INA237::reset()
{
    return _bus.write16(
               _address,
               static_cast<uint8_t>(INA237Defs::Register::CONFIG),
               0x8000) == I2CStatus::OK;
}

void INA237::setCalibration(uint32_t,
                            uint32_t max_current_ma)
{
    _current_lsb =
        (max_current_ma / 1000.0f) / 32768.0f;

    _power_lsb =
        25.0f * _current_lsb;
}

float INA237::readBusVoltage()
{
    uint16_t raw = 0;

    if (_bus.read16(
            _address,
            static_cast<uint8_t>(INA237Defs::Register::VBUS),
            raw) != I2CStatus::OK)
    {
        return 0.0f;
    }

    return raw * 0.003125f;
}

float INA237::readShuntVoltage()
{
    uint16_t raw = 0;

    if (_bus.read16(
            _address,
            static_cast<uint8_t>(INA237Defs::Register::VSHUNT),
            raw) != I2CStatus::OK)
    {
        return 0.0f;
    }

    return static_cast<int16_t>(raw) * 0.0000025f;
}

float INA237::readCurrent()
{
    uint16_t raw = 0;

    if (_bus.read16(
            _address,
            static_cast<uint8_t>(INA237Defs::Register::CURRENT),
            raw) != I2CStatus::OK)
    {
        return 0.0f;
    }

    return static_cast<int16_t>(raw) * _current_lsb;
}

float INA237::readPower()
{
    uint16_t raw = 0;

    if (_bus.read16(
            _address,
            static_cast<uint8_t>(INA237Defs::Register::POWER),
            raw) != I2CStatus::OK)
    {
        return 0.0f;
    }

    return raw * _power_lsb;
}

float INA237::readTemperature()
{
    uint16_t raw = 0;

    if (_bus.read16(
            _address,
            static_cast<uint8_t>(INA237Defs::Register::DIETEMP),
            raw) != I2CStatus::OK)
    {
        return 0.0f;
    }

    return raw * 0.0078125f;
}