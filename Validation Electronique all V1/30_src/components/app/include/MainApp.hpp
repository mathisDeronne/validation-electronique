// ============================================================================
// FILE: components/app/include/MainApp.hpp
// LAYER: APP (orchestrateur)
// PURPOSE: Machine à états principale (FSM) du système de monitoring AGV
// SOURCE: Diagramme elec_FSM.pdf (États 000→005 + erreurs 101→105)
// ============================================================================

#ifndef MAIN_APP_HPP
#define MAIN_APP_HPP

#include "common_types.hpp"

// Couche LOW (drivers)
#include "drv_i2c.hpp"
#include "drv_ina237.hpp"
#include "drv_temp_sensor.hpp"
#include "drv_ihm.hpp"
#include "psram_manager.hpp"
#include "flash_storage.hpp"

// Couche MID (communication)
#include "i2c_manager.hpp"
#include "ble_manager.hpp"
#include "usb_manager.hpp"

// Services APP
#include "MonitoringService.hpp"
#include "AlarmService.hpp"
#include "EventLoggingService.hpp"
#include "ConfigService.hpp"

namespace app {

// Orchestrateur global : possède tous les drivers/services et fait tourner
// la FSM. Cycle (cf. elec_FSM.pdf) :
//   POWER_SELECT(000) → TEST_PROGRAM(001) → INIT_HARDWARE(002)
//      → INIT_COMM(003) → START_APPMAIN(004) → RUN(005)
// Chaque échec bascule vers l'état d'erreur 101..105 (log + bp clear).
class MainApp {
public:
    MainApp();

    common::Status init();
    void run();
    void step();                       // un tick (exposé pour les tests)
    common::SystemState getState() const;

private:
    common::SystemState m_state;

    // --- Couche LOW ---
    low::DrvI2c        m_i2c;
    low::DrvIna237     m_ina237;
    low::DrvTempSensor m_ntc1;
    low::DrvTempSensor m_ntc2;
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

    // Handlers par état
    void handlePowerSelect();   // 000
    void handleTestProgram();   // 001
    void handleInitHardware();  // 002
    void handleInitComm();      // 003
    void handleStartAppmain();  // 004
    void handleRun();           // 005
    void handleError();         // 101..105

    // Tests de chaque étape
    common::Status testProgram();
    common::Status testInitHardware();
    common::Status testComm();
    common::Status testAppmain();

    void logError(common::SystemState faulty_state);
};

} // namespace app

#endif // MAIN_APP_HPP
