// ============================================================================
// FILE: components/app/AlarmService.cpp
// LAYER: APP (service métier)
// PURPOSE: Implémentation du mapping niveau d'alarme → IHM
// MISRA: switch avec default défensif
// ============================================================================

#include "AlarmService.hpp"

namespace app {

AlarmService::AlarmService(low::DrvIhm& ihm)
    : m_ihm(ihm)
{
}

common::Status AlarmService::init()
{
    return m_ihm.init();
}

void AlarmService::showLevel(common::AlarmLevel level)
{
    switch (level)
    {
        case common::AlarmLevel::NORMAL:
            m_ihm.setNotification(common::HmiMode::LED_GREEN_ON);
            break;

        case common::AlarmLevel::WARNING:
            m_ihm.setNotification(common::HmiMode::LED_ORANGE_BLINK_BUZZER_SLOW);
            break;

        case common::AlarmLevel::CRITICAL:
            m_ihm.setNotification(common::HmiMode::LED_RED_ON_BUZZER_FAST);
            break;

        default:
            m_ihm.setNotification(common::HmiMode::LED_RED_ON_BUZZER_FAST);
            break;
    }
}

void AlarmService::showMode(common::HmiMode mode)
{
    m_ihm.setNotification(mode);
}

bool AlarmService::isResetPressed()
{
    return m_ihm.isResetPressed();
}

} // namespace app
