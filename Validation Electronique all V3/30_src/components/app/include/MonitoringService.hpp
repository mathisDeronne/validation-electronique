// ============================================================================
// FILE: components/app/include/MonitoringService.hpp
// LAYER: APP (service métier)
// PURPOSE: Acquiert les mesures (courant + 3 températures) et calcule l'alarme
// CAPTEURS TEMPÉRATURE:
//   - NTC 1 (B600 NB12K00103JBB) → ADC → température PCB zone 1
//   - NTC 2 (B601 NB12K00103JBB) → ADC → température PCB zone 2
//   - TMP126 (B501 TMP126DCKR)   → SPI → température ambiante châssis
// REQUIS: EXF-07, EXF-09, EXF-11, EXF-13, EXF-25
// ============================================================================

#ifndef MONITORING_SERVICE_HPP
#define MONITORING_SERVICE_HPP

#include <cstdint>
#include "common_types.hpp"
#include "drv_ina237.hpp"
#include "drv_temp_sensor.hpp"
#include "drv_tmp126.hpp"

namespace app {

struct MonitorData {
    int32_t  current_ma;
    uint32_t bus_voltage_mv;
    common::float32_t ntc1_temp_c;     // NTC 1 : température PCB zone 1
    common::float32_t ntc2_temp_c;     // NTC 2 : température PCB zone 2
    common::float32_t ambient_temp_c;  // TMP126 : température ambiante (EXF-13)
    common::AlarmLevel level;
    bool     is_valid;
};

class MonitoringService {
public:
    MonitoringService(low::DrvIna237& ina237,
                      low::DrvNtcSensor& ntc1,
                      low::DrvNtcSensor& ntc2,
                      low::DrvTmp126& tmp126);

    common::Status init();
    MonitorData update();
    common::AlarmLevel getLevel() const;

private:
    low::DrvIna237&     m_ina237;
    low::DrvNtcSensor&  m_ntc1;      // NTC PCB zone 1 (EXF-11)
    low::DrvNtcSensor&  m_ntc2;      // NTC PCB zone 2 (EXF-11)
    low::DrvTmp126&     m_tmp126;    // Température ambiante (EXF-13)
    common::AlarmLevel  m_lastLevel;
};

} // namespace app

#endif // MONITORING_SERVICE_HPP
