// ============================================================================
// FILE: components/app/app_power_monitor_logic.cpp
// LAYER: APP (logique pure)
// PURPOSE: Implémentation des décisions d'alarme (sans matériel)
// MISRA: accolades, sortie unique, init systématique
// ============================================================================

#include "app_power_monitor_logic.hpp"
#include "AppConfig.hpp"

namespace app::logic {

bool checkCurrentAlarm(int32_t current_ma, int32_t threshold_ma)
{
    bool alarm = false;

    if (current_ma >= threshold_ma)
    {
        alarm = true;
    }
    else
    {
        alarm = false;
    }

    return alarm;
}

common::AlarmLevel evaluateLevel(int32_t current_ma,
                                 common::float32_t ntc_temp_c,
                                 common::float32_t tmp_temp_c)
{
    common::AlarmLevel level = common::AlarmLevel::NORMAL;

    common::float32_t max_temp = ntc_temp_c;
    if (tmp_temp_c > ntc_temp_c)
    {
        max_temp = tmp_temp_c;
    }

    if ((current_ma >= config::app::CURRENT_CRITICAL_MA) ||
        (max_temp >= config::app::TEMP_CRITICAL_C))
    {
        level = common::AlarmLevel::CRITICAL;
    }
    else if ((current_ma >= config::app::CURRENT_WARNING_MA) ||
             (max_temp >= config::app::TEMP_WARNING_C))
    {
        level = common::AlarmLevel::WARNING;
    }
    else
    {
        level = common::AlarmLevel::NORMAL;
    }

    return level;
}

} // namespace app::logic
