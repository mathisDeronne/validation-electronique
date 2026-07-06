// ============================================================================
// FILE: components/config/include/HardwareConfig.hpp
// LAYER: CONFIG
// PURPOSE: Constantes MATÉRIELLES (broches GPIO, adresses I2C, pins SPI)
// MISRA: pas de magic number
// ============================================================================

#ifndef HARDWARE_CONFIG_HPP
#define HARDWARE_CONFIG_HPP

#include <cstdint>

namespace config::hardware {

// ---- Adresses I2C (EXF-31 : pas de conflit) ----
constexpr uint8_t INA237_I2C_ADDR     = 0x40U;
constexpr uint8_t TMP_SENSOR_I2C_ADDR = 0x48U;

// ---- Broches GPIO LED / Buzzer / Boutons ----
constexpr uint8_t GPIO_LED_GREEN  = 25U;
constexpr uint8_t GPIO_LED_RED    = 26U;
constexpr uint8_t GPIO_BUZZER     = 27U;
constexpr uint8_t GPIO_BTN_RESET  = 32U;
constexpr uint8_t GPIO_BTN_BOOT   = 33U;

// ---- Entrées ADC pour les sondes NTC (NB12K00103JBB) ----
// Schéma : diviseur résistif R_FIXED=10K + R_SERIES=5.1K + filtrage 47nF
constexpr uint8_t ADC_NTC_1 = 34U;  // MES_NTC_1 (B600)
constexpr uint8_t ADC_NTC_2 = 35U;  // MES_NTC_2 (B601)

// ---- Constantes du diviseur résistif NTC ----
constexpr float   NTC_R_FIXED       = 10000.0F;  // R604/R606 = 10K (pull-up vers 3V3)
constexpr float   NTC_R_SERIES      = 5100.0F;   // R605/R607 = 5.1K (série)
constexpr float   NTC_R_NOMINAL     = 10000.0F;  // NB12K00103JBB = 10K @ 25°C
constexpr float   NTC_BETA          = 3435.0F;    // Coefficient Beta (datasheet)
constexpr float   NTC_VCC_MV        = 3300.0F;    // Alimentation 3V3 en mV

// ---- Pins SPI pour TMP126DCKR (B501) ----
constexpr uint8_t SPI_TMP126_CS     = 5U;   // GPIO5  - Chip Select
constexpr uint8_t SPI_TMP126_MOSI   = 23U;  // GPIO23 - Master Out Slave In
constexpr uint8_t SPI_TMP126_MISO   = 19U;  // GPIO19 - Master In Slave Out
constexpr uint8_t SPI_TMP126_SCK    = 18U;  // GPIO18 - Serial Clock

} // namespace config::hardware

#endif // HARDWARE_CONFIG_HPP
