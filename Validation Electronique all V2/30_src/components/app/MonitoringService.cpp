// ============================================================================
// FILE: components/app/MonitoringService.cpp
// LAYER: APP (service métier)
// ============================================================================

#include "MonitoringService.hpp"
#include "app_power_monitor_logic.hpp"

namespace app {

MonitoringService::MonitoringService(low::DrvIna237& ina237,
                                     low::DrvTempSensor& ntc1,
                                     low::DrvTempSensor& ntc2)
    : m_ina237(ina237)
    , m_ntc1(ntc1)
    , m_ntc2(ntc2)
    , m_lastLevel(common::AlarmLevel::NORMAL)
{
}

common::Status MonitoringService::init()
{
    common::Status result = common::Status::OK;

    if (m_ina237.configure() != common::Status::OK)
    {
        result = common::Status::ERROR_INIT;
    }
    else if (m_ntc1.init() != common::Status::OK)
    {
        result = common::Status::ERROR_INIT;
    }
    else if (m_ntc2.init() != common::Status::OK)
    {
        result = common::Status::ERROR_INIT;
    }
    else
    {
        result = common::Status::OK;
    }

    return result;
}

MonitorData MonitoringService::update()
{
    MonitorData data = { 0, 0U, 0.0F, 0.0F, common::AlarmLevel::NORMAL, false };

    const low::Ina237Measurement meas = m_ina237.readMeasurement();

    data.ntc_temp_c = m_ntc1.readTemperatureC();
    data.tmp_temp_c = m_ntc2.readTemperatureC();

    if (meas.is_valid)
    {
        data.current_ma     = meas.current_ma;
        data.bus_voltage_mv = meas.bus_voltage_mv;
        data.level = app::logic::evaluateLevel(meas.current_ma,
                                               data.ntc_temp_c,
                                               data.tmp_temp_c);
        data.is_valid = true;
        m_lastLevel = data.level;
    }

    return data;
}

common::AlarmLevel MonitoringService::getLevel() const
{
    return m_lastLevel;
}

} // namespace app
