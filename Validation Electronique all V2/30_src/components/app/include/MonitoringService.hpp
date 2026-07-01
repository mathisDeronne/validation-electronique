// ============================================================================
// FILE: components/app/include/MonitoringService.hpp
// LAYER: APP (service métier)
// PURPOSE: Acquiert les mesures (courant + température) et calcule l'alarme
// REQUIS: EXF-07, EXF-09, EXF-11, EXF-13, EXF-25
// ============================================================================

#ifndef MONITORING_SERVICE_HPP
#define MONITORING_SERVICE_HPP

#include <cstdint>
#include "common_types.hpp"
#include "drv_ina237.hpp"
#include "drv_temp_sensor.hpp"

namespace app {

struct MonitorData {
    int32_t  current_ma;
    uint32_t bus_voltage_mv;
    common::float32_t ntc_temp_c;
    common::float32_t tmp_temp_c;
    common::AlarmLevel level;
    bool     is_valid;
};

class MonitoringService {
public:
    MonitoringService(low::DrvIna237& ina237,
                      low::DrvTempSensor& ntc1,
                      low::DrvTempSensor& ntc2);

    common::Status init();
    MonitorData update();
    common::AlarmLevel getLevel() const;

private:
    low::DrvIna237&     m_ina237;
    low::DrvTempSensor& m_ntc1;
    low::DrvTempSensor& m_ntc2;
    common::AlarmLevel  m_lastLevel;
};

} // namespace app

#endif // MONITORING_SERVICE_HPP
