// ============================================================================
// FILE: components/low/hal/adc/drv_adc.cpp
// LAYER: LOW (HAL - ADC)
// STATUT: SQUELETTE
// ============================================================================

#include "drv_adc.hpp"

// Sur cible : #include "driver/adc.h"

namespace low {

DrvAdc::DrvAdc() : m_initialized(false) {}

common::Status DrvAdc::init()
{
    // TODO: adc1_config_width(...), adc1_config_channel_atten(...)
    m_initialized = true;
    return common::Status::OK;
}

uint16_t DrvAdc::read(uint8_t channel)
{
    uint16_t raw = 0U;
    if (m_initialized)
    {
        // raw = (uint16_t)adc1_get_raw((adc1_channel_t)channel);
        (void)channel;
    }
    return raw;
}

} // namespace low
