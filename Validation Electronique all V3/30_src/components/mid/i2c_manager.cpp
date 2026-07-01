// ============================================================================
// FILE: components/mid/i2c_manager.cpp
// LAYER: MID (communication)
// PURPOSE: Table d'adresses I2C + init bus via ESP-IDF
// ============================================================================

#include "i2c_manager.hpp"

// Sur cible ESP32 réelle :
// #include "driver/i2c.h"

namespace mid {

I2cManager::I2cManager()
    : m_addresses{ 0U }
    , m_count(0U)
    , m_initialized(false)
{
}

common::Status I2cManager::init()
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // i2c_config_t conf = { ... };
    // esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
    // if (err == ESP_OK) { err = i2c_driver_install(...); }
    // result = (err == ESP_OK) ? OK : ERROR_I2C;

    m_initialized = (result == common::Status::OK);

    return result;
}

common::Status I2cManager::registerDevice(uint8_t addr)
{
    common::Status result = common::Status::OK;

    if (isAddressUsed(addr))
    {
        result = common::Status::ERROR; // EXF-31 : conflit
    }
    else if (m_count >= MAX_DEVICES)
    {
        result = common::Status::ERROR;
    }
    else
    {
        m_addresses[m_count] = addr;
        m_count++;
    }

    return result;
}

bool I2cManager::isAddressUsed(uint8_t addr) const
{
    bool used = false;

    for (size_t i = 0U; i < m_count; i++)
    {
        if (m_addresses[i] == addr)
        {
            used = true;
        }
    }

    return used;
}

size_t I2cManager::getDeviceCount() const
{
    return m_count;
}

} // namespace mid
