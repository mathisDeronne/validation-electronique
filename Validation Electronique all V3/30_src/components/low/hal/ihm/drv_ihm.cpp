// ============================================================================
// FILE: components/low/hal/ihm/drv_ihm.cpp
// LAYER: LOW (HAL - IHM)
// PURPOSE: Implémentation pilotage LED/buzzer/boutons
// NOTE: Les appels GPIO ESP-IDF sont en commentaire (mock sur PC).
// ============================================================================

#include "drv_ihm.hpp"

// Sur cible réelle :
// #include "driver/gpio.h"
// #include "HardwareConfig.hpp"

namespace low {

DrvIhm::DrvIhm()
    : m_initialized(false)
{
}

common::Status DrvIhm::init()
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ---
    // gpio_set_direction(GPIO_LED_GREEN, GPIO_MODE_OUTPUT); ... etc.

    m_initialized = true;
    setNotification(common::HmiMode::OFF);

    return result;
}

void DrvIhm::setNotification(common::HmiMode mode)
{
    if (!m_initialized)
    {
        // Sécurité : ne rien faire si non initialisé
    }
    else
    {
        switch (mode)
        {
            case common::HmiMode::OFF:
                writeLedGreen(false);
                writeLedRed(false);
                writeBuzzer(false);
                break;

            case common::HmiMode::SUCCESS_BEEP:
                writeLedGreen(true);
                writeLedRed(false);
                writeBuzzer(true);
                break;

            case common::HmiMode::LED_GREEN_ON:
                writeLedGreen(true);
                writeLedRed(false);
                writeBuzzer(false);
                break;

            case common::HmiMode::LED_ORANGE_BLINK_BUZZER_SLOW:
                writeLedGreen(true);
                writeLedRed(true);
                writeBuzzer(true);
                break;

            case common::HmiMode::LED_RED_ON_BUZZER_FAST:
                writeLedGreen(false);
                writeLedRed(true);
                writeBuzzer(true);
                break;

            default:
                writeLedGreen(false);
                writeLedRed(true);
                writeBuzzer(true);
                break;
        }
    }
}

bool DrvIhm::isResetPressed()
{
    bool pressed = false;
    if (m_initialized)
    {
        // pressed = (gpio_get_level(GPIO_BTN_RESET) == 0);
        pressed = false; // Mock
    }
    return pressed;
}

bool DrvIhm::isBootPressed()
{
    bool pressed = false;
    if (m_initialized)
    {
        // pressed = (gpio_get_level(GPIO_BTN_BOOT) == 0);
        pressed = false; // Mock
    }
    return pressed;
}

void DrvIhm::writeLedGreen(bool on) { (void)on; }
void DrvIhm::writeLedRed(bool on)   { (void)on; }
void DrvIhm::writeBuzzer(bool on)   { (void)on; }

} // namespace low
