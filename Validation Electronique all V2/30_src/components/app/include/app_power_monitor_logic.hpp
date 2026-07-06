// ============================================================================
// FILE: components/app/include/app_power_monitor_logic.hpp
// LAYER: APP (logique pure)
// PURPOSE: Décision d'alarme à partir des mesures (sans aucun I/O)
// MISRA: fonctions pures, testables sur PC
// REQUIS: EXF-09, EXF-25
// ============================================================================

#ifndef APP_POWER_MONITOR_LOGIC_HPP
#define APP_POWER_MONITOR_LOGIC_HPP

#include <cstdint>
#include "common_types.hpp"

namespace app::logic {

// Alarme courant : true si current_ma >= threshold_ma (négatif = pas d'alarme)
bool checkCurrentAlarm(int32_t current_ma, int32_t threshold_ma);

// Niveau combiné courant + température (prend la temp la plus haute)
common::AlarmLevel evaluateLevel(int32_t current_ma,
                                 common::float32_t ntc_temp_c,
                                 common::float32_t tmp_temp_c);

} // namespace app::logic

#endif // APP_POWER_MONITOR_LOGIC_HPP
