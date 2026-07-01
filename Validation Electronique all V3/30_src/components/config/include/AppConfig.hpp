// ============================================================================
// FILE: components/config/include/AppConfig.hpp
// LAYER: CONFIG
// PURPOSE: Constantes MÉTIER (seuils d'alarme, fréquences d'échantillonnage)
//          Utilisées par la couche APP.
// MISRA: pas de magic number
// ============================================================================

#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include <cstdint>
#include "common_types.hpp"

namespace config::app {

// ---- Seuils de courant en mA (EXF-09, EXF-25) ----
constexpr int32_t CURRENT_WARNING_MA  = 15000; // 15 A
constexpr int32_t CURRENT_CRITICAL_MA = 20000; // 20 A (EXF-25)
constexpr int32_t CURRENT_MAX_MA      = 30000; // 30 A (EXF-09)

// ---- Seuils de température en °C (EXF-25 : PCB > 75 °C) ----
constexpr common::float32_t TEMP_WARNING_C    = 60.0F;
constexpr common::float32_t TEMP_CRITICAL_C   = 75.0F;
constexpr common::float32_t TEMP_HYSTERESIS_C = 5.0F;

} // namespace config::app

#endif // APP_CONFIG_HPP
