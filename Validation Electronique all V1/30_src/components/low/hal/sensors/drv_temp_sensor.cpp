// ============================================================================
// FILE: components/low/hal/sensors/drv_temp_sensor.cpp
// LAYER: LOW (HAL - capteur température)
// STATUT: SQUELETTE — renvoie une valeur mock pour l'instant
// ============================================================================

#include "drv_temp_sensor.hpp"

namespace low {

DrvTempSensor::DrvTempSensor(uint8_t adc_pin)
    : m_adcPin(adc_pin)
    , m_initialized(false)
{
}

common::Status DrvTempSensor::init()
{
    // TODO: configurer l'entrée ADC associée (via DrvAdc).
    m_initialized = true;
    return common::Status::OK;
}

common::float32_t DrvTempSensor::readTemperatureC()
{
    common::float32_t temp_c = 25.0F; // Mock

    if (m_initialized)
    {
        // TODO (cible réelle) :
        //  1. raw = adc.read(m_adcPin);
        //  2. r_ntc = resistanceFromAdc(raw);
        //  3. temp_c = steinhartHart(r_ntc);
        (void)m_adcPin;
    }

    return temp_c;
}

} // namespace low
