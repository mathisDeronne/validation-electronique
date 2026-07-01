// ============================================================================
// FILE: components/mid/include/i2c_manager.hpp
// LAYER: MID (communication)
// PURPOSE: Arbitrage du bus I2C + vérification des adresses (EXF-31)
// MISRA: pas de conflit d'adresse documenté
// ============================================================================

#ifndef I2C_MANAGER_HPP
#define I2C_MANAGER_HPP

#include <cstdint>
#include <cstddef>
#include "common_types.hpp"
#include "drv_i2c.hpp"

namespace mid {

// Gère le bus I2C partagé et tient la table des adresses (EXF-31).
class I2cManager {
public:
    explicit I2cManager(low::DrvI2c& bus);

    common::Status init();

    // Enregistre une adresse et vérifie qu'elle n'est pas déjà prise (EXF-31).
    common::Status registerDevice(uint8_t addr);

    // Vrai si l'adresse a déjà été enregistrée (détection de conflit).
    bool isAddressUsed(uint8_t addr) const;

    size_t getDeviceCount() const;

private:
    low::DrvI2c& m_bus;

    // Table statique des adresses (MISRA : pas d'allocation dynamique)
    static constexpr size_t MAX_DEVICES = 8U;
    uint8_t m_addresses[MAX_DEVICES];
    size_t  m_count;
};

} // namespace mid

#endif // I2C_MANAGER_HPP
