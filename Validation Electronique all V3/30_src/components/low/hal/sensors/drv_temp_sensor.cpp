// ============================================================================
// FILE: components/low/hal/sensors/drv_temp_sensor.cpp
// LAYER: LOW (HAL - capteur température NTC)
// PURPOSE: Lecture NTC NB12K00103JBB via ADC + conversion en °C
//
// Schéma électrique (pour chaque NTC) :
//
//   3V3 ──[R604 10K]──┬──[R605 5.1K]──► MES_NTC_1 (ADC ESP32)
//                      │                      │
//                   [C604 47nF]           [C603 47nF]
//                      │                      │
//                   [B600 NTC]               GND
//                      │
//                     GND
//
// R604 (10K) = pull-up vers 3V3
// B600 (NTC) = thermistance 10K @ 25°C, vers GND
// R605 (5.1K) = résistance série de protection (n'affecte pas la mesure)
// C603, C604 = filtrage 47nF (EXF-12)
//
// Formule :
//   V_adc = VCC × R_NTC / (R_FIXED + R_NTC)
//   R_NTC = R_FIXED × V_adc / (VCC - V_adc)
//   T(°C) = 1 / (1/T0 + ln(R_NTC/R0) / Beta) - 273.15
// ============================================================================

#include "drv_temp_sensor.hpp"
#include "HardwareConfig.hpp"
#include <cmath>

// Sur cible ESP32 réelle :
// #include "driver/adc.h"
// #include "esp_adc_cal.h"

namespace low {

DrvNtcSensor::DrvNtcSensor(uint8_t adc_channel)
    : m_channel(adc_channel)
    , m_initialized(false)
{
}

common::Status DrvNtcSensor::init()
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_config_channel_atten((adc1_channel_t)m_channel, ADC_ATTEN_DB_11);

    (void)m_channel;
    m_initialized = true;

    return result;
}

common::float32_t DrvNtcSensor::readTemperatureC()
{
    common::float32_t temp_c = 25.0F;

    if (m_initialized)
    {
        const uint16_t raw = readRawAdc();
        const common::float32_t r_ntc = adcToResistance(raw);
        temp_c = resistanceToTemperature(r_ntc);
    }

    return temp_c;
}

uint16_t DrvNtcSensor::readRawAdc()
{
    uint16_t raw = 2048U; // Mock : milieu de plage (12 bits = 0..4095)

    // --- Cible réelle ESP-IDF ---
    // raw = static_cast<uint16_t>(adc1_get_raw((adc1_channel_t)m_channel));

    return raw;
}

// ============================================================================
// Conversion ADC brut → résistance NTC
//
// Le diviseur résistif :  V_adc = VCC × R_NTC / (R_FIXED + R_NTC)
// Donc :                  R_NTC = R_FIXED × V_adc / (VCC - V_adc)
//
// En valeur ADC (12 bits, 0-4095) :
//   R_NTC = R_FIXED × raw / (4095 - raw)
// ============================================================================
common::float32_t DrvNtcSensor::adcToResistance(uint16_t raw_adc)
{
    common::float32_t r_ntc = config::hardware::NTC_R_NOMINAL;

    if ((raw_adc > 0U) && (raw_adc < 4095U))
    {
        const common::float32_t raw_f = static_cast<common::float32_t>(raw_adc);
        r_ntc = config::hardware::NTC_R_FIXED * raw_f / (4095.0F - raw_f);
    }

    return r_ntc;
}

// ============================================================================
// Conversion résistance NTC → température en °C (formule Beta)
//
//   1/T = 1/T0 + (1/Beta) × ln(R_NTC / R0)
//
//   T0 = 298.15 K (25°C)
//   R0 = 10000 Ω (NB12K00103JBB = 10K @ 25°C)
//   Beta = 3435 (datasheet)
// ============================================================================
common::float32_t DrvNtcSensor::resistanceToTemperature(common::float32_t r_ntc)
{
    common::float32_t temp_c = 25.0F;

    if (r_ntc > 0.0F)
    {
        constexpr common::float32_t T0_KELVIN = 298.15F;  // 25°C en Kelvin

        const common::float32_t inv_t =
            (1.0F / T0_KELVIN) +
            (1.0F / config::hardware::NTC_BETA) *
            std::log(r_ntc / config::hardware::NTC_R_NOMINAL);

        temp_c = (1.0F / inv_t) - 273.15F;  // Kelvin → Celsius
    }

    return temp_c;
}

} // namespace low
