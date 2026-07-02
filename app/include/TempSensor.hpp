#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <math.h>
#include <esp_adc/adc_oneshot.h>

//==============================================================
// NTC NB12K00103JBB
//==============================================================

constexpr float ADC_MAX = 4095.0f;
constexpr float VCC = 3.3f;

// Résistance fixe
constexpr float R_FIXED = 10000.0f;

// NTC
constexpr float R0 = 10000.0f;
constexpr float T0 = 298.15f; // 25°C
constexpr float BETA = 3435.0f;

//==============================================================
// ADC -> Température
//==============================================================

inline float adcToTemperature(int adc)
{
    if (adc <= 0)
        adc = 1;

    if (adc >= 4095)
        adc = 4094;

    // Tension mesurée
    float voltage = adc * VCC / ADC_MAX;

    // Calcul de la résistance de la NTC
    float r_ntc = (R_FIXED * voltage) / (VCC - voltage);

    // Equation de Beta
    float temperature =
        1.0f /
        ((1.0f / T0) +
         (1.0f / BETA) * log(r_ntc / R0));

    return temperature - 273.15f;
}

#endif