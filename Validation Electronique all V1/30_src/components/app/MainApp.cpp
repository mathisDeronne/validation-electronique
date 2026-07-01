// ============================================================================
// FILE: components/app/MainApp.cpp
// LAYER: APP (orchestrateur)
// PURPOSE: Implémentation de la FSM principale (diagramme elec_FSM.pdf)
// MISRA: accolades, sortie unique, fonctions courtes, default défensif
// ============================================================================

#include "MainApp.hpp"
#include "HardwareConfig.hpp"

namespace app {

// Construction : démarrage OBLIGATOIRE en POWER_SELECT (000).
// Toutes les dépendances sont injectées par référence (pas de new).
MainApp::MainApp()
    : m_state(common::SystemState::POWER_SELECT)
    , m_i2c()
    , m_ina237(m_i2c, config::hardware::INA237_I2C_ADDR)
    , m_ntc1(config::hardware::ADC_NTC_1)
    , m_ntc2(config::hardware::ADC_NTC_2)
    , m_ihm()
    , m_psram()
    , m_flash()
    , m_i2cMgr(m_i2c)
    , m_ble()
    , m_usb()
    , m_monitoring(m_ina237, m_ntc1, m_ntc2)
    , m_alarm(m_ihm)
    , m_events(m_psram)
    , m_config(m_flash)
    , m_running(false)
{
}

// ============================================================================
// INITIALISATION GLOBALE
// ============================================================================
common::Status MainApp::init()
{
    common::Status result = common::Status::OK;

    // IHM d'abord (pour pouvoir signaler les erreurs)
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
        // Enregistrement des adresses I2C (EXF-31 : détection de conflit)
        (void)m_i2cMgr.registerDevice(config::hardware::INA237_I2C_ADDR);
        (void)m_i2cMgr.registerDevice(config::hardware::TMP_SENSOR_I2C_ADDR);

        (void)m_config.init();
        (void)m_events.init();

        m_state = common::SystemState::POWER_SELECT;
        result = common::Status::OK;
    }

    return result;
}

// ============================================================================
// BOUCLE PRINCIPALE
// ============================================================================
void MainApp::run()
{
    m_running = true;

    while (m_running)
    {
        step();
        // Sur cible : vTaskDelay(pdMS_TO_TICKS(config::system::APP_PERIOD_MS));
    }
}

// ============================================================================
// UN TICK DE FSM
// ============================================================================
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

// ============================================================================
// HANDLERS PAR ÉTAT
// ============================================================================

// État 000 : choix alim + alimentation ESP
void MainApp::handlePowerSelect()
{
    // Le choix de source (USB > BT > HT) est MATÉRIEL (EXF-03).
    m_alarm.showMode(common::HmiMode::OFF);
    m_state = common::SystemState::TEST_PROGRAM;
}

// État 001 : test programme
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

// État 002 : init hardware (RAZ mémoires, périph, GPIO, fréq échant)
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

// État 003 : démarrage communications BLE + I2C
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

// État 004 : démarrage programme appmain
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

// État 005 : vie programme (boucle de monitoring nominale)
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
        // Pilotage IHM (EXF-24, EXF-25)
        m_alarm.showLevel(data.level);

        // Remontée BLE (EXF-19)
        m_ble.updateCurrent(data.current_ma);
        m_ble.notifyAlarm(data.level);

        // Journalisation (EXF-15, EXF-16)
        m_events.logMeasurement(data.current_ma, data.ntc_temp_c);

        // Traitement des commandes USB (EXF-21)
        m_usb.processIncomingCommands();

        // bp clear : le reset relance un cycle de test
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

// États 101..105 : gestion d'erreur (log + attente bp clear)
void MainApp::handleError()
{
    m_alarm.showMode(common::HmiMode::LED_RED_ON_BUZZER_FAST);

    if (m_alarm.isResetPressed())
    {
        m_state = common::SystemState::TEST_PROGRAM;
    }
    else
    {
        // On reste dans l'état d'erreur courant
    }
}

// ============================================================================
// TESTS DE CHAQUE ÉTAPE
// ============================================================================

common::Status MainApp::testProgram()
{
    // Vérification d'intégrité firmware (CRC, version). Mock : OK.
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
        result = common::Status::OK;
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

// ============================================================================
// JOURNALISATION
// ============================================================================
void MainApp::logError(common::SystemState faulty_state)
{
    // EXF-17 : log système. Mock pour l'instant.
    (void)faulty_state;
}

} // namespace app
