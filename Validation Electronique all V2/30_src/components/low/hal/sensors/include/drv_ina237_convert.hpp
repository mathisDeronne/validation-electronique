// ============================================================================
// FILE: components/low/hal/sensors/include/drv_ina237_convert.hpp
// LAYER: LOW (HAL - capteur, partie pure)
// PURPOSE: Conversions registres bruts INA237 → unités physiques (mV/mA/mW)
// MISRA: fonctions pures, aucun I/O, testables sur PC
// REQUIS: EXF-07, EXF-09
// ============================================================================

#ifndef DRV_INA237_CONVERT_HPP
#define DRV_INA237_CONVERT_HPP

#include <cstdint>

namespace low::ina237 {

// VBUS brut → millivolts (1 LSB = 3.125 mV)
uint32_t convertVbusRawToMv(uint16_t raw);

// Courant brut signé → milliampères
int32_t convertCurrentRawToMa(int16_t raw, uint32_t current_lsb_ua);

// Puissance brute → milliwatts (Power_LSB = 25 × Current_LSB)
uint32_t convertPowerRawToMw(uint16_t raw, uint32_t current_lsb_ua);

} // namespace low::ina237

#endif // DRV_INA237_CONVERT_HPP
