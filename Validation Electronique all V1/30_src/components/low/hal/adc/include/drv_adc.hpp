// ============================================================================
// FILE: components/low/hal/adc/include/drv_adc.hpp
// LAYER: LOW (HAL - ADC)
// PURPOSE: Lecture des entrées analogiques ESP32 (sondes NTC)
// STATUT: SQUELETTE — encapsulera driver/adc.h
// ============================================================================

#ifndef DRV_ADC_HPP
#define DRV_ADC_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class DrvAdc {
public:
    DrvAdc();
    common::Status init();
    // Renvoie la valeur brute (0..4095 sur 12 bits)
    uint16_t read(uint8_t channel);
private:
    bool m_initialized;
};

} // namespace low

#endif // DRV_ADC_HPP
