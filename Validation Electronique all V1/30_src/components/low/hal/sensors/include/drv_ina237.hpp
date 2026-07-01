// ============================================================================
// FILE: components/low/hal/sensors/include/drv_ina237.hpp
// LAYER: LOW (HAL - capteur courant)
// PURPOSE: Lecture/écriture registres INA237 via le bus I2C
// REQUIS: EXF-07, EXF-08, EXF-09, EXF-10
// ============================================================================

#ifndef DRV_INA237_HPP
#define DRV_INA237_HPP

#include <cstdint>
#include "common_types.hpp"
#include "drv_i2c.hpp"

namespace low {

// Mesure complète (valeurs physiques brutes)
struct Ina237Measurement {
    int32_t  current_ma;
    uint32_t bus_voltage_mv;
    uint32_t power_mw;
    bool     is_valid;
};

class DrvIna237 {
public:
    explicit DrvIna237(DrvI2c& i2c_bus, uint8_t i2c_addr);

    common::Status configure();
    common::Status readCurrentMa(int32_t& out_current_ma);
    Ina237Measurement readMeasurement();
    bool selfTest(); // EXF-10

private:
    DrvI2c&  m_i2c;
    uint8_t  m_addr;
    uint32_t m_currentLsbUa;

    static constexpr uint8_t REG_CONFIG  = 0x00U;
    static constexpr uint8_t REG_VBUS    = 0x05U;
    static constexpr uint8_t REG_CURRENT = 0x07U;
    static constexpr uint8_t REG_POWER   = 0x08U;
};

} // namespace low

#endif // DRV_INA237_HPP
