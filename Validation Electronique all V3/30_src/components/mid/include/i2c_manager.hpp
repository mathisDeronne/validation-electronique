// ============================================================================
// FILE: components/mid/include/i2c_manager.hpp
// LAYER: MID (communication)
// PURPOSE: Table des adresses I2C — détection de conflits (EXF-31)
//          Ne gère PAS le bus I2C lui-même (ESP-IDF s'en charge).
// ============================================================================

#ifndef I2C_MANAGER_HPP
#define I2C_MANAGER_HPP

#include <cstdint>
#include <cstddef>
#include "common_types.hpp"

namespace mid {

class I2cManager {
public:
    I2cManager();

    // Initialise le bus I2C via ESP-IDF
    common::Status init();

    // Enregistre une adresse et vérifie qu'elle n'est pas déjà prise (EXF-31)
    common::Status registerDevice(uint8_t addr);

    bool isAddressUsed(uint8_t addr) const;
    size_t getDeviceCount() const;

private:
    static constexpr size_t MAX_DEVICES = 8U;
    uint8_t m_addresses[MAX_DEVICES];
    size_t  m_count;
    bool    m_initialized;
};

} // namespace mid

#endif // I2C_MANAGER_HPP
