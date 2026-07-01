// ============================================================================
// FILE: components/mid/i2c_manager.cpp
// LAYER: MID (communication)
// PURPOSE: Implémentation de la gestion du bus I2C et table d'adresses
// ============================================================================

#include "i2c_manager.hpp"

namespace mid {

I2cManager::I2cManager(low::DrvI2c& bus)
    : m_bus(bus)
    , m_addresses{ 0U }
    , m_count(0U)
{
}

common::Status I2cManager::init()
{
    return m_bus.init();
}

common::Status I2cManager::registerDevice(uint8_t addr)
{
    common::Status result = common::Status::OK;

    if (isAddressUsed(addr))
    {
        // EXF-31 : conflit d'adresse détecté
        result = common::Status::ERROR;
    }
    else if (m_count >= MAX_DEVICES)
    {
        result = common::Status::ERROR;
    }
    else
    {
        m_addresses[m_count] = addr;
        m_count++;
        result = common::Status::OK;
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
