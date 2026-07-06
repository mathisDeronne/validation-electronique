// ============================================================================
// FILE: components/low/hal/sensors/drv_temp_sensor.cpp
// LAYER: LOW (HAL - capteur température)
// STATUT: SQUELETTE — appelle ESP-IDF ADC directement
// ============================================================================

#include "drv_temp_sensor.hpp"

// Sur cible ESP32 réelle :
// #include "driver/adc.h"
// #include "esp_adc_cal.h"

namespace low {

DrvTempSensor::DrvTempSensor(uint8_t adc_channel)
    : m_channel(adc_channel)
    , m_initialized(false)
{
}

common::Status DrvTempSensor::init()
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_config_channel_atten((adc1_channel_t)m_channel, ADC_ATTEN_DB_11);

    (void)m_channel;
    m_initialized = true;

    return result;
}

common::float32_t DrvTempSensor::readTemperatureC()
{
    common::float32_t temp_c = 25.0F; // Mock

    if (m_initialized)
    {
        // --- Cible réelle ESP-IDF ---
        // int raw = adc1_get_raw((adc1_channel_t)m_channel);
        // float voltage_mv = (float)esp_adc_cal_raw_to_voltage(raw, &adc_chars);
        // float r_ntc = (voltage_mv * R_FIXED) / (VCC_MV - voltage_mv);
        // temp_c = steinhartHart(r_ntc);
    }

    return temp_c;
}

} // namespace low
