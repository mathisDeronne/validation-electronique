// ============================================================================
// FILE: components/app/MonitoringService.cpp
// LAYER: APP (service métier)
// PURPOSE: Acquisition des 3 températures + courant, calcul alarme
// ============================================================================

#include "MonitoringService.hpp"
#include "app_power_monitor_logic.hpp"

namespace app {

MonitoringService::MonitoringService(low::DrvIna237& ina237,
                                     low::DrvNtcSensor& ntc1,
                                     low::DrvNtcSensor& ntc2,
                                     low::DrvTmp126& tmp126)
    : m_ina237(ina237)
    , m_ntc1(ntc1)
    , m_ntc2(ntc2)
    , m_tmp126(tmp126)
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
    else if (m_tmp126.init() != common::Status::OK)
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
    MonitorData data = { 0, 0U, 0.0F, 0.0F, 0.0F, common::AlarmLevel::NORMAL, false };

    // 1. Lecture capteur courant (INA237 via I2C)
    const low::Ina237Measurement meas = m_ina237.readMeasurement();

    // 2. Lecture des 3 températures
    data.ntc1_temp_c    = m_ntc1.readTemperatureC();     // NTC PCB zone 1 (ADC)
    data.ntc2_temp_c    = m_ntc2.readTemperatureC();     // NTC PCB zone 2 (ADC)
    data.ambient_temp_c = m_tmp126.readTemperatureC();   // Ambiante (SPI)

    if (meas.is_valid)
    {
        data.current_ma     = meas.current_ma;
        data.bus_voltage_mv = meas.bus_voltage_mv;

        // 3. Alarme basée sur les 2 NTC (température PCB, EXF-25 : > 75°C)
        // Le TMP126 mesure l'ambiante : utile pour le monitoring mais pas
        // directement pour l'alarme PCB.
        data.level = app::logic::evaluateLevel(meas.current_ma,
                                               data.ntc1_temp_c,
                                               data.ntc2_temp_c);
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
