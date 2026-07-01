// ============================================================================
// FILE: components/app/MainApp.cpp
// LAYER: APP (orchestrateur)
// PURPOSE: Implémentation de la FSM principale (diagramme elec_FSM.pdf)
// ============================================================================

#include "MainApp.hpp"
#include "HardwareConfig.hpp"

namespace app {

MainApp::MainApp()
    : m_state(common::SystemState::POWER_SELECT)
    , m_ina237(config::hardware::INA237_I2C_ADDR)
    , m_ntc1(config::hardware::ADC_NTC_1)
    , m_ntc2(config::hardware::ADC_NTC_2)
    , m_tmp126()
    , m_ihm()
    , m_psram()
    , m_flash()
    , m_i2cMgr()
    , m_ble()
    , m_usb()
    , m_monitoring(m_ina237, m_ntc1, m_ntc2, m_tmp126)
    , m_alarm(m_ihm)
    , m_events(m_psram)
    , m_config(m_flash)
    , m_running(false)
{
}

common::Status MainApp::init()
{
    common::Status result = common::Status::OK;

    if (m_alarm.init() != common::Status::OK)
    {
        result = common::Status::ERROR_INIT;
    }
    else if (m_i2cMgr.init() != common::Status::OK)
    {
        result = common::Status::ERROR_I2C;
    }
    else
    {
        (void)m_i2cMgr.registerDevice(config::hardware::INA237_I2C_ADDR);
        (void)m_i2cMgr.registerDevice(config::hardware::TMP_SENSOR_I2C_ADDR);

        (void)m_config.init();
        (void)m_events.init();

        m_state = common::SystemState::POWER_SELECT;
    }

    return result;
}

void MainApp::run()
{
    m_running = true;

    while (m_running)
    {
        step();
    }
}

void MainApp::step()
{
    switch (m_state)
    {
        case common::SystemState::POWER_SELECT:
            handlePowerSelect();
            break;

        case common::SystemState::TEST_PROGRAM:
            handleTestProgram();
            break;

        case common::SystemState::INIT_HARDWARE:
            handleInitHardware();
            break;

        case common::SystemState::INIT_COMM:
            handleInitComm();
            break;

        case common::SystemState::START_APPMAIN:
            handleStartAppmain();
            break;

        case common::SystemState::RUN:
            handleRun();
            break;

        case common::SystemState::ERR_PROGRAM:
        case common::SystemState::ERR_INIT:
        case common::SystemState::ERR_COMM:
        case common::SystemState::ERR_APPMAIN:
        case common::SystemState::ERR_RUNTIME:
            handleError();
            break;

        default:
            logError(m_state);
            m_state = common::SystemState::ERR_RUNTIME;
            break;
    }
}

common::SystemState MainApp::getState() const
{
    return m_state;
}

void MainApp::handlePowerSelect()
{
    m_alarm.showMode(common::HmiMode::OFF);
    m_state = common::SystemState::TEST_PROGRAM;
}

void MainApp::handleTestProgram()
{
    if (testProgram() == common::Status::OK)
    {
        m_state = common::SystemState::INIT_HARDWARE;
    }
    else
    {
        m_state = common::SystemState::ERR_PROGRAM;
    }
}

void MainApp::handleInitHardware()
{
    if (testInitHardware() == common::Status::OK)
    {
        m_state = common::SystemState::INIT_COMM;
    }
    else
    {
        m_state = common::SystemState::ERR_INIT;
    }
}

void MainApp::handleInitComm()
{
    if (testComm() == common::Status::OK)
    {
        m_state = common::SystemState::START_APPMAIN;
    }
    else
    {
        m_state = common::SystemState::ERR_COMM;
    }
}

void MainApp::handleStartAppmain()
{
    if (testAppmain() == common::Status::OK)
    {
        m_state = common::SystemState::RUN;
    }
    else
    {
        m_state = common::SystemState::ERR_APPMAIN;
    }
}

void MainApp::handleRun()
{
    const MonitorData data = m_monitoring.update();

    if (!data.is_valid)
    {
        logError(common::SystemState::RUN);
        m_state = common::SystemState::ERR_RUNTIME;
    }
    else
    {
        m_alarm.showLevel(data.level);
        m_ble.updateCurrent(data.current_ma);
        m_ble.updateTemperatures(data.ntc1_temp_c, data.ntc2_temp_c,
                                 data.ambient_temp_c);
        m_ble.notifyAlarm(data.level);
        m_events.logMeasurement(data.current_ma, data.ntc1_temp_c);
        m_usb.processIncomingCommands();

        if (m_alarm.isResetPressed())
        {
            m_state = common::SystemState::TEST_PROGRAM;
        }
        else
        {
            m_state = common::SystemState::RUN;
        }
    }
}

void MainApp::handleError()
{
    m_alarm.showMode(common::HmiMode::LED_RED_ON_BUZZER_FAST);

    if (m_alarm.isResetPressed())
    {
        m_state = common::SystemState::TEST_PROGRAM;
    }
}

common::Status MainApp::testProgram()
{
    return common::Status::OK;
}

common::Status MainApp::testInitHardware()
{
    common::Status result = common::Status::OK;

    if (m_monitoring.init() != common::Status::OK)
    {
        result = common::Status::ERROR_INIT;
    }
    else if (!m_ina237.selfTest())
    {
        result = common::Status::ERROR_INIT;
    }
    else if (!m_tmp126.selfTest())
    {
        result = common::Status::ERROR_INIT;
    }
    else
    {
        result = common::Status::OK;
    }

    return result;
}

common::Status MainApp::testComm()
{
    common::Status result = common::Status::OK;

    if (m_ble.init() != common::Status::OK)
    {
        result = common::Status::ERROR;
    }
    else if (m_usb.init() != common::Status::OK)
    {
        result = common::Status::ERROR;
    }
    else if (!m_ina237.selfTest())
    {
        result = common::Status::ERROR_I2C;
    }
    else
    {
        m_ble.startAdvertising();
    }

    return result;
}

common::Status MainApp::testAppmain()
{
    common::Status result = common::Status::OK;
    const MonitorData data = m_monitoring.update();

    if (!data.is_valid)
    {
        result = common::Status::ERROR;
    }

    return result;
}

void MainApp::logError(common::SystemState faulty_state)
{
    (void)faulty_state;
}

} // namespace app
