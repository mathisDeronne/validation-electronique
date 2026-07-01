// ============================================================================
// FILE: components/config/include/SystemConfig.hpp
// LAYER: CONFIG
// PURPOSE: Configuration GÉNÉRALE de l'application (périodes, conversions)
// MISRA: pas de magic number
// ============================================================================

#ifndef SYSTEM_CONFIG_HPP
#define SYSTEM_CONFIG_HPP

#include <cstdint>

namespace config::system {

// ---- Période de la boucle principale FSM ----
constexpr uint32_t APP_PERIOD_MS = 100U; // 10 Hz

// ---- Conversion INA237 (datasheet Texas Instruments) ----
constexpr uint32_t INA237_VBUS_LSB_UV   = 3125U; // 3.125 mV
constexpr uint32_t INA237_POWER_LSB_NUM = 25U;   // Power_LSB = 25 × Current_LSB
constexpr uint32_t INA237_CURRENT_LSB_UA = 1000U; // 1 mA / LSB (plage 0-30A)

} // namespace config::system

#endif // SYSTEM_CONFIG_HPP
