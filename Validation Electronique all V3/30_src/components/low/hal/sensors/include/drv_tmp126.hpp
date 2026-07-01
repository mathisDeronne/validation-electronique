// ============================================================================
// FILE: components/low/hal/sensors/include/drv_tmp126.hpp
// LAYER: LOW (HAL - capteur température SPI)
// PURPOSE: Lecture du capteur TMP126DCKR via SPI
//
// Schéma électrique :
//   GPIO5  (CS)   → pin 2 (CS)    du TMP126
//   GPIO23 (MOSI) → pin 3 (SIO)   du TMP126
//   GPIO19 (MISO) → pin 3 (SIO)   du TMP126 (bidirectionnel)
//   GPIO18 (SCK)  → pin 4 (SCLK)  du TMP126
//   3V3           → pin 6 (VDD)   + R500(10K) pull-up + C500(0.1µF) découplage
//   GND           → pin 1 (GND)
//   pin 5 (ALERT) → non connecté (pour l'instant)
//
// REQUIS: EXF-13 (capteur numérique température ambiante)
// ============================================================================

#ifndef DRV_TMP126_HPP
#define DRV_TMP126_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class DrvTmp126 {
public:
    DrvTmp126();

    // Configure le bus SPI et vérifie la présence du capteur
    common::Status init();

    // Lit la température en °C (résolution 14 bits, précision ±0.3°C)
    common::float32_t readTemperatureC();

    // Vérifie que le capteur répond (lecture du device ID)
    bool selfTest();

private:
    bool m_initialized;

    // Communication SPI bas niveau (appelle ESP-IDF directement)
    common::Status spiRead(uint8_t reg, uint16_t& out_value);
    common::Status spiWrite(uint8_t reg, uint16_t value);

    // Registres TMP126 (datasheet Texas Instruments)
    static constexpr uint8_t REG_TEMP      = 0x00U;  // Température (14 bits)
    static constexpr uint8_t REG_CONFIG    = 0x01U;  // Configuration
    static constexpr uint8_t REG_DEVICE_ID = 0x0FU;  // ID du capteur
};

} // namespace low

#endif // DRV_TMP126_HPP
