// ============================================================================
// FILE: components/app/include/MainApp.hpp
// LAYER: APP (orchestrateur)
// PURPOSE: FSM principale — diagramme elec_FSM.pdf
// ============================================================================

#ifndef MAIN_APP_HPP
#define MAIN_APP_HPP

#include "common_types.hpp"

// Couche LOW
#include "drv_ina237.hpp"
#include "drv_temp_sensor.hpp"
#include "drv_tmp126.hpp"
#include "drv_ihm.hpp"
#include "psram_manager.hpp"
#include "flash_storage.hpp"

// Couche MID
#include "i2c_manager.hpp"
#include "ble_manager.hpp"
#include "usb_manager.hpp"

// Services APP
#include "MonitoringService.hpp"
#include "AlarmService.hpp"
#include "EventLoggingService.hpp"
#include "ConfigService.hpp"

namespace app {

class MainApp {
public:
    MainApp();

    common::Status init();
    void run();
    void step();
    common::SystemState getState() const;

private:
    common::SystemState m_state;

    // --- Couche LOW ---
    low::DrvIna237     m_ina237;
    low::DrvNtcSensor  m_ntc1;       // NTC PCB zone 1 (B600)
    low::DrvNtcSensor  m_ntc2;       // NTC PCB zone 2 (B601)
    low::DrvTmp126     m_tmp126;     // Température ambiante (B501)
    low::DrvIhm        m_ihm;
    low::PsramManager  m_psram;
    low::FlashStorage  m_flash;

    // --- Couche MID ---
    mid::I2cManager m_i2cMgr;
    mid::BleManager m_ble;
    mid::UsbManager m_usb;

    // --- Services APP ---
    MonitoringService   m_monitoring;
    AlarmService        m_alarm;
    EventLoggingService m_events;
    ConfigService       m_config;

    bool m_running;

    void handlePowerSelect();
    void handleTestProgram();
    void handleInitHardware();
    void handleInitComm();
    void handleStartAppmain();
    void handleRun();
    void handleError();

    common::Status testProgram();
    common::Status testInitHardware();
    common::Status testComm();
    common::Status testAppmain();

    void logError(common::SystemState faulty_state);
};

} // namespace app

#endif // MAIN_APP_HPP
