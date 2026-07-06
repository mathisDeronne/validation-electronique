// ============================================================================
// FILE: components/app/ConfigService.cpp
// LAYER: APP (service métier)
// STATUT: SQUELETTE
// ============================================================================

#include "ConfigService.hpp"
#include "AppConfig.hpp"

namespace app {

ConfigService::ConfigService(low::FlashStorage& flash)
    : m_flash(flash)
    , m_currentThresholdMa(config::app::CURRENT_CRITICAL_MA)
{
}

common::Status ConfigService::init()
{
    common::Status result = m_flash.init();

    if (result == common::Status::OK)
    {
        // TODO: charger le seuil depuis la flash s'il existe
        int32_t stored = 0;
        if (m_flash.readInt32("cur_thr", stored) == common::Status::OK)
        {
            // m_currentThresholdMa = stored; (si valeur valide)
            (void)stored;
        }
    }

    return result;
}

int32_t ConfigService::getCurrentThresholdMa() const
{
    return m_currentThresholdMa;
}

void ConfigService::setCurrentThresholdMa(int32_t value)
{
    m_currentThresholdMa = value;
    (void)m_flash.writeInt32("cur_thr", value);
}

} // namespace app
