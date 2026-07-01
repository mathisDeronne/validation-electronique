// ============================================================================
// FILE: components/low/hal/sensors/drv_ina237.cpp
// LAYER: LOW (HAL - capteur courant)
// PURPOSE: Implémentation du driver INA237 (ESP-IDF I2C direct)
// ============================================================================

#include "drv_ina237.hpp"
#include "drv_ina237_convert.hpp"
#include "SystemConfig.hpp"

// Sur cible ESP32 réelle :
// #include "driver/i2c.h"

namespace low {

DrvIna237::DrvIna237(uint8_t i2c_addr)
    : m_addr(i2c_addr)
    , m_currentLsbUa(config::system::INA237_CURRENT_LSB_UA)
    , m_configured(false)
{
}

common::Status DrvIna237::configure()
{
    common::Status result = common::Status::OK;

    result = i2cWriteReg(REG_CONFIG, 0x4127U);

    if (result == common::Status::OK)
    {
        m_configured = true;
    }

    return result;
}

common::Status DrvIna237::readCurrentMa(int32_t& out_current_ma)
{
    common::Status result = common::Status::OK;
    uint16_t raw = 0U;

    result = i2cReadReg(REG_CURRENT, raw);

    if (result == common::Status::OK)
    {
        const int16_t raw_signed = static_cast<int16_t>(raw);
        out_current_ma = ina237::convertCurrentRawToMa(raw_signed, m_currentLsbUa);
    }
    else
    {
        out_current_ma = 0;
    }

    return result;
}

Ina237Measurement DrvIna237::readMeasurement()
{
    Ina237Measurement meas = { 0, 0U, 0U, false };
    uint16_t raw_current = 0U;
    uint16_t raw_vbus    = 0U;
    uint16_t raw_power   = 0U;

    const common::Status s1 = i2cReadReg(REG_CURRENT, raw_current);
    const common::Status s2 = i2cReadReg(REG_VBUS, raw_vbus);
    const common::Status s3 = i2cReadReg(REG_POWER, raw_power);

    if ((s1 == common::Status::OK) &&
        (s2 == common::Status::OK) &&
        (s3 == common::Status::OK))
    {
        meas.current_ma     = ina237::convertCurrentRawToMa(
                                  static_cast<int16_t>(raw_current), m_currentLsbUa);
        meas.bus_voltage_mv = ina237::convertVbusRawToMv(raw_vbus);
        meas.power_mw       = ina237::convertPowerRawToMw(raw_power, m_currentLsbUa);
        meas.is_valid       = true;
    }

    return meas;
}

bool DrvIna237::selfTest()
{
    return i2cPing();
}

// ============================================================================
// I2C BAS NIVEAU — appelle ESP-IDF directement
// Sur cible réelle, décommenter les appels i2c_master_*
// ============================================================================

common::Status DrvIna237::i2cWriteReg(uint8_t reg, uint16_t value)
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // uint8_t payload[3] = { reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFFU) };
    // esp_err_t err = i2c_master_write_to_device(
    //     I2C_NUM_0, m_addr, payload, 3, pdMS_TO_TICKS(100));
    // result = (err == ESP_OK) ? common::Status::OK : common::Status::ERROR_I2C;

    (void)reg;
    (void)value;

    return result;
}

common::Status DrvIna237::i2cReadReg(uint8_t reg, uint16_t& out_value)
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // uint8_t raw[2] = {0U, 0U};
    // esp_err_t err = i2c_master_write_read_device(
    //     I2C_NUM_0, m_addr, &reg, 1, raw, 2, pdMS_TO_TICKS(100));
    // out_value = static_cast<uint16_t>((raw[0] << 8) | raw[1]);
    // result = (err == ESP_OK) ? common::Status::OK : common::Status::ERROR_I2C;

    (void)reg;
    out_value = 0U;

    return result;
}

bool DrvIna237::i2cPing()
{
    bool present = false;

    // --- Cible réelle ESP-IDF ---
    // esp_err_t err = i2c_master_write_to_device(
    //     I2C_NUM_0, m_addr, nullptr, 0, pdMS_TO_TICKS(50));
    // present = (err == ESP_OK);

    present = true; // Mock

    return present;
}

} // namespace low
