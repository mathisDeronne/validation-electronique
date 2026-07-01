// ============================================================================
// FILE: components/low/hal/i2c/drv_i2c.cpp
// LAYER: LOW (HAL - bus I2C)
// PURPOSE: Implémentation du pilote I2C
// NOTE: Les appels ESP-IDF (i2c_master_*) sont en commentaire (mock sur PC).
// ============================================================================

#include "drv_i2c.hpp"

// Sur cible ESP32 réelle :
// #include "driver/i2c.h"

namespace low {

DrvI2c::DrvI2c()
    : m_initialized(false)
{
}

common::Status DrvI2c::init()
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // i2c_config_t conf = { ... };
    // if (i2c_param_config(I2C_NUM_0, &conf) != ESP_OK) { result = ERROR_I2C; }
    // else if (i2c_driver_install(...) != ESP_OK)       { result = ERROR_I2C; }

    m_initialized = (result == common::Status::OK);

    return result;
}

common::Status DrvI2c::writeRegister(uint8_t device_addr, uint8_t reg, uint16_t value)
{
    common::Status result = common::Status::OK;

    if (!m_initialized)
    {
        result = common::Status::ERROR_INIT;
    }
    else
    {
        // --- Cible réelle ---
        // uint8_t payload[3] = { reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFFU) };
        // result = (i2c_master_write_to_device(...) == ESP_OK) ? OK : ERROR_I2C;
        (void)device_addr;
        (void)reg;
        (void)value;
    }

    return result;
}

common::Status DrvI2c::readRegister(uint8_t device_addr, uint8_t reg, uint16_t& out_value)
{
    common::Status result = common::Status::OK;

    if (!m_initialized)
    {
        result = common::Status::ERROR_INIT;
    }
    else
    {
        // --- Cible réelle ---
        // uint8_t raw[2] = {0U, 0U};
        // result = (i2c_master_write_read_device(...) == ESP_OK) ? OK : ERROR_I2C;
        // out_value = (uint16_t)((raw[0] << 8) | raw[1]);
        (void)device_addr;
        (void)reg;
        out_value = 0U;
    }

    return result;
}

bool DrvI2c::isDevicePresent(uint8_t device_addr)
{
    bool present = false;

    if (m_initialized)
    {
        (void)device_addr;
        present = true; // Mock
    }

    return present;
}

} // namespace low
