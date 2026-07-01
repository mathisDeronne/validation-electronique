// ============================================================================
// FILE: components/low/hal/i2c/include/drv_i2c.hpp
// LAYER: LOW (HAL - bus I2C)
// PURPOSE: Pilote générique du bus I2C de l'ESP32 (encapsule driver/i2c.h)
// MISRA: seule couche autorisée à inclure les headers ESP-IDF
// ============================================================================

#ifndef DRV_I2C_HPP
#define DRV_I2C_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class DrvI2c {
public:
    DrvI2c();

    common::Status init();
    common::Status writeRegister(uint8_t device_addr, uint8_t reg, uint16_t value);
    common::Status readRegister(uint8_t device_addr, uint8_t reg, uint16_t& out_value);
    bool isDevicePresent(uint8_t device_addr);

private:
    bool m_initialized;
};

} // namespace low

#endif // DRV_I2C_HPP
