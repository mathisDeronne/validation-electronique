// ============================================================================
// FILE: components/config/include/HardwareConfig.hpp
// LAYER: CONFIG
// PURPOSE: Constantes MATÉRIELLES (broches GPIO, adresses I2C)
//          Utilisées EXCLUSIVEMENT par la couche LOW (drivers).
// MISRA: pas de magic number
// ============================================================================

#ifndef HARDWARE_CONFIG_HPP
#define HARDWARE_CONFIG_HPP

#include <cstdint>

namespace config::hardware {

// ---- Adresses I2C (EXF-31 : pas de conflit) ----
constexpr uint8_t INA237_I2C_ADDR     = 0x40U; // Capteur courant
constexpr uint8_t TMP_SENSOR_I2C_ADDR = 0x48U; // Capteur température numérique (EXF-13)

// ---- Broches GPIO (EXF-24, EXF-25, EXF-26, EXF-33) ----
constexpr uint8_t GPIO_LED_GREEN  = 25U;
constexpr uint8_t GPIO_LED_RED    = 26U;
constexpr uint8_t GPIO_BUZZER     = 27U;
constexpr uint8_t GPIO_BTN_RESET  = 32U;
constexpr uint8_t GPIO_BTN_BOOT   = 33U;

// ---- Entrées ADC pour les sondes NTC (EXF-11, EXF-12) ----
constexpr uint8_t ADC_NTC_1 = 34U;
constexpr uint8_t ADC_NTC_2 = 35U;

} // namespace config::hardware

#endif // HARDWARE_CONFIG_HPP
