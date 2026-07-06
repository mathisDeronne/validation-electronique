// ============================================================================
// FILE: components/low/hal/sensors/include/drv_temp_sensor.hpp
// LAYER: LOW (HAL - capteur température)
// PURPOSE: Lecture des sondes NTC (EXF-11, EXF-12, EXF-14)
// STATUT: SQUELETTE — à compléter avec lecture ADC + conversion Steinhart-Hart
// ============================================================================

#ifndef DRV_TEMP_SENSOR_HPP
#define DRV_TEMP_SENSOR_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

// Deux sondes NTC mesurent la température du PCB (EXF-11).
// Chaque sonde : diviseur résistif + filtrage capacitif, lue sur ADC (EXF-12).
class DrvTempSensor {
public:
    // adc_pin : entrée ADC associée à la sonde
    explicit DrvTempSensor(uint8_t adc_pin);

    common::Status init();

    // Renvoie la température en °C (toujours en Celsius côté LOW).
    // TODO: lire l'ADC, calculer la résistance NTC, appliquer Steinhart-Hart.
    common::float32_t readTemperatureC();

private:
    uint8_t m_adcPin;
    bool    m_initialized;
};

} // namespace low

#endif // DRV_TEMP_SENSOR_HPP
