// ============================================================================
// FILE: components/low/hal/sensors/drv_ina237_convert.cpp
// LAYER: LOW (HAL - capteur, partie pure)
// PURPOSE: Implémentation des conversions INA237 (sans matériel)
// MISRA: point de sortie unique, init systématique
// ============================================================================

#include "drv_ina237_convert.hpp"

namespace low::ina237 {

uint32_t convertVbusRawToMv(uint16_t raw)
{
    uint32_t result_mv = 0U;
    const uint32_t raw_u32 = static_cast<uint32_t>(raw);

    result_mv = (raw_u32 * 3125U) / 1000U;

    return result_mv;
}

int32_t convertCurrentRawToMa(int16_t raw, uint32_t current_lsb_ua)
{
    int32_t result_ma = 0;
    const int32_t raw_i32 = static_cast<int32_t>(raw);
    const int32_t lsb_i32 = static_cast<int32_t>(current_lsb_ua);

    result_ma = (raw_i32 * lsb_i32) / 1000;

    return result_ma;
}

uint32_t convertPowerRawToMw(uint16_t raw, uint32_t current_lsb_ua)
{
    uint32_t result_mw = 0U;
    const uint32_t raw_u32 = static_cast<uint32_t>(raw);
    const uint32_t power_lsb_ua = 25U * current_lsb_ua;

    result_mw = (raw_u32 * power_lsb_ua) / 1000U;

    return result_mw;
}

} // namespace low::ina237
