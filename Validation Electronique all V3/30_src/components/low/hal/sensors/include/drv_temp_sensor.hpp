// ============================================================================
// FILE: components/low/hal/sensors/include/drv_temp_sensor.hpp
// LAYER: LOW (HAL - capteur température NTC)
// PURPOSE: Lecture des sondes NTC NB12K00103JBB via ADC ESP32
//          Schéma : 3V3 → R_FIXED(10K) → point de mesure → R_SERIES(5.1K) → NTC → GND
//          Filtrage : 2x condensateurs 47nF
// REQUIS: EXF-11, EXF-12, EXF-14
// ============================================================================

#ifndef DRV_TEMP_SENSOR_HPP
#define DRV_TEMP_SENSOR_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class DrvNtcSensor {
public:
    // adc_channel : entrée ADC associée à la sonde (ADC_NTC_1 ou ADC_NTC_2)
    explicit DrvNtcSensor(uint8_t adc_channel);

    common::Status init();

    // Lit l'ADC, calcule la résistance NTC, convertit en °C (formule Beta)
    common::float32_t readTemperatureC();

    // Lecture brute ADC (pour debug / calibration)
    uint16_t readRawAdc();

private:
    uint8_t m_channel;
    bool    m_initialized;

    // Conversion interne
    common::float32_t adcToResistance(uint16_t raw_adc);
    common::float32_t resistanceToTemperature(common::float32_t r_ntc);
};

} // namespace low

#endif // DRV_TEMP_SENSOR_HPP
