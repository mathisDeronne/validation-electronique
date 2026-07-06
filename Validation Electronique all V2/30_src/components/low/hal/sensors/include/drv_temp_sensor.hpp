// ============================================================================
// FILE: components/low/hal/sensors/include/drv_temp_sensor.hpp
// LAYER: LOW (HAL - capteur température)
// PURPOSE: Lecture des sondes NTC via ESP-IDF ADC directement
//          (pas de wrapper drv_adc — ESP-IDF fournit déjà le driver ADC)
// REQUIS: EXF-11, EXF-12, EXF-14
// STATUT: SQUELETTE — à compléter avec conversion Steinhart-Hart
// ============================================================================

#ifndef DRV_TEMP_SENSOR_HPP
#define DRV_TEMP_SENSOR_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class DrvTempSensor {
public:
    explicit DrvTempSensor(uint8_t adc_channel);

    common::Status init();

    // Renvoie la température en °C (toujours Celsius côté LOW)
    common::float32_t readTemperatureC();

private:
    uint8_t m_channel;
    bool    m_initialized;
};

} // namespace low

#endif // DRV_TEMP_SENSOR_HPP
