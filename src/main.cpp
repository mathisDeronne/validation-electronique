// ============================================================================
// main.cpp — Ventec Systems Monitoring Card
// Conforme au CDC Ventec Systems & Énoncé Projet ESP32
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_err.h>
#include <esp_timer.h>
#include "esp_log.h"

#include "HardwareConfig.hpp"
#include "TempSensor.hpp"
#include "buzzermelody.hpp"
#include "blemanager.hpp"
#include "SPIManager.hpp"
#include "I2CBus.hpp"
#include "INA237.hpp"

static const char *TAG = "VENTEC";

// ============================================================================
// SEUILS D'ALARME (CDC EXF-25 / EXF-27 / EXF-23)
// Modifiables à chaud via BLE sans reprogrammation (EXF-23)
// ============================================================================
static float s_currentWarningA  = 15.0f;   // Warning courant
static float s_currentCriticalA = 20.0f;   // EXF-25 : critique courant
static float s_tempWarningC     = 60.0f;   // Warning température
static float s_tempCriticalC    = 75.0f;   // EXF-25 : critique température

// ============================================================================
// SYSTÈME DE LOGS CIRCULAIRE (CDC EXF-15 à EXF-18)
// ============================================================================
// EXF-15 : historique horodaté courant + températures
// EXF-16 : alarmes avec timestamp + valeur de déclenchement
// EXF-17 : logs système (redémarrages, connexions technicien)
// EXF-18 : rotation circulaire (écrasement des plus anciens)

enum class LogType : uint8_t
{
    SYSTEM  = 0,   // Redémarrage, init, changement alimentation
    MEASURE = 1,   // Mesure périodique courant/température
    ALARM   = 2,   // Alarme déclenchée avec valeur
    BLE     = 3    // Connexion/déconnexion technicien
};

struct LogEntry
{
    uint32_t timestamp_s;
    LogType  type;
    float    value;
    char     message[48];
};

static constexpr size_t LOG_BUFFER_SIZE = 256;
static LogEntry s_logs[LOG_BUFFER_SIZE];
static size_t   s_log_head  = 0;
static size_t   s_log_count = 0;

static uint32_t uptimeSeconds()
{
    return static_cast<uint32_t>(esp_timer_get_time() / 1000000ULL);
}

static void addLog(LogType type, float value, const char *msg)
{
    LogEntry &entry   = s_logs[s_log_head];
    entry.timestamp_s = uptimeSeconds();
    entry.type        = type;
    entry.value       = value;

    strncpy(entry.message, msg, sizeof(entry.message) - 1);
    entry.message[sizeof(entry.message) - 1] = '\0';

    s_log_head = (s_log_head + 1) % LOG_BUFFER_SIZE;
    if (s_log_count < LOG_BUFFER_SIZE)
        s_log_count++;

    const char *typeStr = "SYS";
    if (type == LogType::MEASURE) typeStr = "MES";
    if (type == LogType::ALARM)   typeStr = "ALM";
    if (type == LogType::BLE)     typeStr = "BLE";

    printf("[LOG %s T+%lus] %.2f | %s\n",
           typeStr, (unsigned long)entry.timestamp_s, value, msg);
}

// ============================================================================
// NIVEAUX D'ALARME (CDC EXF-25 / EXF-27)
// ============================================================================
enum class AlarmLevel : uint8_t
{
    NONE     = 0,   // Fonctionnement normal
    WARNING  = 1,   // Avertissement : LED rouge clignotante
    CRITICAL = 2    // Critique : LED rouge fixe + buzzer
};

static AlarmLevel s_alarmLevel     = AlarmLevel::NONE;
static AlarmLevel s_prevAlarmLevel = AlarmLevel::NONE;
static bool       s_bleWasConnected = false;

// ============================================================================
// PROTOTYPES
// ============================================================================
static AlarmLevel evaluateAlarms(float temp1, float temp2, float current);
static void       sendBleData(float temp1, float temp2,
                              float spiTemp, bool spiOk,
                              float busV, float current, float power,
                              bool inaOk, AlarmLevel alarm);
static void       sendBleLastLogs(size_t count);

// ============================================================================
// MAIN
// ============================================================================
extern "C" void app_main(void)
{
    printf("\n");
    printf("=============================================\n");
    printf("  VENTEC SYSTEMS - Carte de Monitoring AGV\n");
    printf("  ESP32-PICO-D4 - Groupe 5\n");
    printf("=============================================\n\n");

    // EXF-17 : log système au démarrage
    addLog(LogType::SYSTEM, 0.0f, "BOOT: Demarrage systeme");

    // ------------------------------------------------------------------
    // GPIO INIT : LED + BUZZER (EXF-24 / EXF-25 / EXF-26)
    // ------------------------------------------------------------------
    gpio_set_direction(HardwareConfig::IHM::LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::BUZZER, GPIO_MODE_OUTPUT);

    // EXF-24 : LED verte active en permanence = fonctionnement normal
    gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);
    gpio_set_level(HardwareConfig::IHM::LED_RED, 0);
    gpio_set_level(HardwareConfig::IHM::BUZZER, 0);

    addLog(LogType::SYSTEM, 0.0f, "GPIO: LEDs et buzzer init");

    // ------------------------------------------------------------------
    // BLE INIT (EXF-19 / EXF-23)
    // ------------------------------------------------------------------
    if (!blemanager::init("Groupe 5"))
    {
        ESP_LOGE(TAG, "BLE init FAILED");
        addLog(LogType::SYSTEM, 0.0f, "ERREUR: Init BLE echouee");
        return;
    }

    blemanager::start();
    addLog(LogType::SYSTEM, 0.0f, "BLE: Init OK, advertising");

    // ------------------------------------------------------------------
    // ADC INIT : NTC sur GPIO25 + GPIO26 (EXF-11 / EXF-12)
    // ------------------------------------------------------------------
    adc_oneshot_unit_handle_t adc_handle = nullptr;
    bool adc_ok = true;

    adc_oneshot_unit_init_cfg_t init_cfg = {};
    init_cfg.unit_id = ADC_UNIT_2;
    init_cfg.ulp_mode = ADC_ULP_MODE_DISABLE;
    init_cfg.clk_src = ADC_RTC_CLK_SRC_DEFAULT;

    if (adc_oneshot_new_unit(&init_cfg, &adc_handle) != ESP_OK)
    {
        adc_ok = false;
        addLog(LogType::SYSTEM, 0.0f, "ERREUR: Init ADC echouee");
    }
    else
    {
        adc_oneshot_chan_cfg_t chan_cfg = {};
        chan_cfg.bitwidth = ADC_BITWIDTH_12;
        chan_cfg.atten = ADC_ATTEN_DB_12;

        if (adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_8, &chan_cfg) != ESP_OK ||
            adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_9, &chan_cfg) != ESP_OK)
        {
            adc_ok = false;
            addLog(LogType::SYSTEM, 0.0f, "ERREUR: Config ADC channels");
        }
        else
        {
            addLog(LogType::SYSTEM, 0.0f, "ADC: NTC1 + NTC2 OK");
        }
    }

    // ------------------------------------------------------------------
    // TMP126 SPI INIT (EXF-13 : température ambiante châssis)
    // ------------------------------------------------------------------
    SPIManager spi;
    bool spi_ok = (spi.init() == ESP_OK);
    addLog(LogType::SYSTEM, 0.0f,
           spi_ok ? "SPI: TMP126 OK" : "ERREUR: TMP126 init");

    // ------------------------------------------------------------------
    // INA237 I2C INIT (EXF-07 / EXF-08 / EXF-09)
    // Mesure continue courant batterie 36V, plage 0-30A
    // ------------------------------------------------------------------
    I2CBus bus;
    bool ina_ok = false;

    bus.begin(21, 22, 400000);
    INA237 ina(bus);

    if (!ina.begin())
    {
        addLog(LogType::SYSTEM, 0.0f, "ERREUR: INA237 absent I2C");
    }
    else
    {
        // EXF-09 : plage 0-30A, détection dérives dès 0.5A
        ina.setCalibration(10000, 2000);
        ina_ok = true;
        addLog(LogType::SYSTEM, 0.0f, "I2C: INA237 OK, calibre");
    }

    addLog(LogType::SYSTEM, 0.0f, "INIT COMPLETE, boucle active");

    // ------------------------------------------------------------------
    // VARIABLES
    // ------------------------------------------------------------------
    int   adc1 = 0, adc2 = 0;
    float temp1 = -999.0f, temp2 = -999.0f, spiTemp = -999.0f;
    float inaBusV = 0.0f, inaCur = 0.0f, inaPwr = 0.0f;

    uint32_t loopCounter  = 0;
    uint32_t logInterval  = 0;
    bool     warnLedState = false;

    // ------------------------------------------------------------------
    // BOUCLE PRINCIPALE — 200ms / itération
    // ------------------------------------------------------------------
    while (true)
    {
        // ==============================================================
        // RÉCEPTION COMMANDES BLE
        // ==============================================================
        if (blemanager::available())
        {
            std::string cmd = blemanager::read();
            printf("CMD BLE => %s\n", cmd.c_str());

            if      (cmd == "LED_GREEN_ON")   gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);
            else if (cmd == "LED_GREEN_OFF")  gpio_set_level(HardwareConfig::IHM::LED_GREEN, 0);
            else if (cmd == "LED_RED_ON")     gpio_set_level(HardwareConfig::IHM::LED_RED, 1);
            else if (cmd == "LED_RED_OFF")    gpio_set_level(HardwareConfig::IHM::LED_RED, 0);
            else if (cmd == "BUZZER_ON")      gpio_set_level(HardwareConfig::IHM::BUZZER, 1);
            else if (cmd == "BUZZER_OFF")     gpio_set_level(HardwareConfig::IHM::BUZZER, 0);
            else if (cmd == "ALARM_1")        playAlarm(AlarmSound::TripleBeep);
            else if (cmd == "ALARM_2")        playAlarm(AlarmSound::Siren);
            else if (cmd == "ALARM_3")        playAlarm(AlarmSound::Warning);
            else if (cmd == "ALARM_4")        playAlarm(AlarmSound::Emergency);
            else if (cmd == "GET_LOGS")       sendBleLastLogs(10);
            // --- EXF-23 : modification des seuils via BLE ---
            else if (cmd.rfind("SET_THRESH=", 0) == 0)
            {
                // Format : SET_THRESH=15.0,20.0,60.0,75.0
                float cw, cc, tw, tc;
                if (sscanf(cmd.c_str() + 11, "%f,%f,%f,%f", &cw, &cc, &tw, &tc) == 4)
                {
                    if (cw > 0 && cc > cw && tw > 0 && tc > tw)
                    {
                        s_currentWarningA  = cw;
                        s_currentCriticalA = cc;
                        s_tempWarningC     = tw;
                        s_tempCriticalC    = tc;

                        char buf[64];
                        snprintf(buf, sizeof(buf),
                                 "Seuils MAJ: I=%.1f/%.1fA T=%.0f/%.0fC",
                                 cw, cc, tw, tc);
                        addLog(LogType::SYSTEM, 0.0f, buf);

                        // Confirmation vers le panneau HTML
                        snprintf(buf, sizeof(buf),
                                 "THRESH=%.1f,%.1f,%.0f,%.0f",
                                 cw, cc, tw, tc);
                        blemanager::notify(std::string(buf));
                    }
                    else
                    {
                        addLog(LogType::SYSTEM, 0.0f, "ERREUR: Seuils invalides");
                        blemanager::notify(std::string("THRESH_ERR=INVALID"));
                    }
                }
            }
            else if (cmd == "GET_THRESH")
            {
                char buf[64];
                snprintf(buf, sizeof(buf), "THRESH=%.1f,%.1f,%.0f,%.0f",
                         s_currentWarningA, s_currentCriticalA,
                         s_tempWarningC, s_tempCriticalC);
                blemanager::notify(std::string(buf));
            }
            else ESP_LOGW(TAG, "Commande inconnue: %s", cmd.c_str());
        }

        // ==============================================================
        // DÉTECTION CONNEXION / DÉCONNEXION BLE (EXF-17)
        // ==============================================================
        bool bleNow = blemanager::isConnected();

        if (bleNow && !s_bleWasConnected)
            addLog(LogType::BLE, 0.0f, "Technicien connecte");
        else if (!bleNow && s_bleWasConnected)
            addLog(LogType::BLE, 0.0f, "Technicien deconnecte");

        s_bleWasConnected = bleNow;

        // ==============================================================
        // LECTURES CAPTEURS
        // ==============================================================

        // NTC1 + NTC2 (EXF-11 / EXF-12)
        if (adc_ok && adc_handle)
        {
            temp1 = (adc_oneshot_read(adc_handle, ADC_CHANNEL_8, &adc1) == ESP_OK)
                    ? adcToTemperature(adc1) : -999.0f;
            temp2 = (adc_oneshot_read(adc_handle, ADC_CHANNEL_9, &adc2) == ESP_OK)
                    ? adcToTemperature(adc2) : -999.0f;
        }

        // TMP126 SPI (EXF-13)
        bool tmp126Ok = false;
        if (spi_ok)
        {
            tmp126Ok = (spi.readTemperature(spiTemp) == ESP_OK);
            if (!tmp126Ok) spiTemp = -999.0f;
        }

        // INA237 I2C (EXF-07)
        if (ina_ok)
        {
            inaBusV = ina.readBusVoltage();
            inaCur  = ina.readCurrent();
            inaPwr  = ina.readPower();
        }

        // ==============================================================
        // ÉVALUATION ALARMES (EXF-25 / EXF-27)
        // ==============================================================
        s_prevAlarmLevel = s_alarmLevel;
        s_alarmLevel     = evaluateAlarms(temp1, temp2, inaCur);

        // EXF-16 : log si changement de niveau avec valeur de déclenchement
        if (s_alarmLevel != s_prevAlarmLevel)
        {
            float maxT = fmaxf(temp1, temp2);
            float absI = fabsf(inaCur);

            if (s_alarmLevel == AlarmLevel::CRITICAL)
            {
                float trig = (absI >= s_currentCriticalA) ? inaCur : maxT;
                char buf[48];
                snprintf(buf, sizeof(buf), "CRITIQUE: val=%.2f", trig);
                addLog(LogType::ALARM, trig, buf);
            }
            else if (s_alarmLevel == AlarmLevel::WARNING)
            {
                float trig = (absI >= s_currentWarningA) ? inaCur : maxT;
                char buf[48];
                snprintf(buf, sizeof(buf), "WARNING: val=%.2f", trig);
                addLog(LogType::ALARM, trig, buf);
            }
            else
            {
                addLog(LogType::ALARM, 0.0f, "Retour normal");
            }
        }

        // ==============================================================
        // SORTIES PHYSIQUES ALARME (EXF-24 / EXF-25 / EXF-26 / EXF-27)
        // ==============================================================
        if (s_alarmLevel == AlarmLevel::CRITICAL)
        {
            // Critique : verte OFF, rouge fixe ON, buzzer ON
            gpio_set_level(HardwareConfig::IHM::LED_GREEN, 0);
            gpio_set_level(HardwareConfig::IHM::LED_RED, 1);
            gpio_set_level(HardwareConfig::IHM::BUZZER, 1);
        }
        else if (s_alarmLevel == AlarmLevel::WARNING)
        {
            // Warning : verte ON, rouge clignotante, buzzer OFF
            gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);
            warnLedState = !warnLedState;
            gpio_set_level(HardwareConfig::IHM::LED_RED, warnLedState ? 1 : 0);
            gpio_set_level(HardwareConfig::IHM::BUZZER, 0);
        }
        else
        {
            // Normal : verte ON, rouge OFF, buzzer OFF
            gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);
            gpio_set_level(HardwareConfig::IHM::LED_RED, 0);
            gpio_set_level(HardwareConfig::IHM::BUZZER, 0);
        }

        // ==============================================================
        // LOG PÉRIODIQUE MESURES (EXF-15)
        // ~30s = 150 × 200ms
        // ==============================================================
        logInterval++;
        if (logInterval >= 150)
        {
            logInterval = 0;
            char buf[48];
            snprintf(buf, sizeof(buf), "I=%.3fA T1=%.1f T2=%.1f",
                     inaCur, temp1, temp2);
            addLog(LogType::MEASURE, inaCur, buf);
        }

        // ==============================================================
        // AFFICHAGE SÉRIE (toutes les ~1s)
        // ==============================================================
        if (loopCounter % 5 == 0)
        {
            printf("\n-----------------------------\n");

            if (temp1 > -900.0f)
                printf("NTC1 (GPIO25) : %4d -> %.2f C\n", adc1, temp1);
            else
                printf("NTC1 (GPIO25) : Erreur\n");

            if (temp2 > -900.0f)
                printf("NTC2 (GPIO26) : %4d -> %.2f C\n", adc2, temp2);
            else
                printf("NTC2 (GPIO26) : Erreur\n");

            if (tmp126Ok)
                printf("TMP126 (SPI)  : %.2f C\n", spiTemp);
            else
                printf("TMP126 (SPI)  : Erreur\n");

            if (ina_ok)
            {
                printf("INA237 Bus    : %.3f V\n", inaBusV);
                printf("INA237 Current: %.3f A\n", inaCur);
                printf("INA237 Power  : %.3f W\n", inaPwr);
            }
            else
                printf("INA237        : Non connecte\n");

            printf("Alarme        : %s\n",
                   s_alarmLevel == AlarmLevel::CRITICAL ? "CRITIQUE" :
                   s_alarmLevel == AlarmLevel::WARNING  ? "WARNING" : "OK");
            printf("BLE           : %s\n", bleNow ? "Connecte" : "Deconnecte");
            printf("Uptime        : %lu s | Logs: %zu/%zu\n",
                   (unsigned long)uptimeSeconds(), s_log_count, LOG_BUFFER_SIZE);
            printf("-----------------------------\n");
        }

        // ==============================================================
        // ENVOI BLE (~1s) (EXF-19)
        // ==============================================================
        if (bleNow && (loopCounter % 5 == 0))
        {
            sendBleData(temp1, temp2, spiTemp, tmp126Ok,
                        inaBusV, inaCur, inaPwr, ina_ok,
                        s_alarmLevel);
        }

        loopCounter++;
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// ============================================================================
// ÉVALUATION DES ALARMES
// ============================================================================
static AlarmLevel evaluateAlarms(float temp1, float temp2, float current)
{
    float absI = fabsf(current);
    float maxT = -999.0f;

    if (temp1 > -900.0f) maxT = temp1;
    if (temp2 > -900.0f && temp2 > maxT) maxT = temp2;

    // EXF-25 : critique si courant >= seuil ou temp PCB >= seuil
    if (absI >= s_currentCriticalA)  return AlarmLevel::CRITICAL;
    if (maxT > -900.0f && maxT >= s_tempCriticalC) return AlarmLevel::CRITICAL;

    // EXF-27 : warning si courant >= seuil ou temp PCB >= seuil
    if (absI >= s_currentWarningA)   return AlarmLevel::WARNING;
    if (maxT > -900.0f && maxT >= s_tempWarningC)  return AlarmLevel::WARNING;

    return AlarmLevel::NONE;
}

// ============================================================================
// ENVOI BLE TEMPS RÉEL (EXF-19)
// ============================================================================
static void sendBleData(float temp1, float temp2,
                        float spiTemp, bool spiOk,
                        float busV, float current, float power,
                        bool inaOk, AlarmLevel alarm)
{
    char msg[256];
    int pos = 0;

    // Températures PCB
    if (temp1 > -900.0f)
        pos += snprintf(msg + pos, sizeof(msg) - pos, "NTC1=%.2f", temp1);
    else
        pos += snprintf(msg + pos, sizeof(msg) - pos, "NTC1=ERR");

    if (temp2 > -900.0f)
        pos += snprintf(msg + pos, sizeof(msg) - pos, ";NTC2=%.2f", temp2);
    else
        pos += snprintf(msg + pos, sizeof(msg) - pos, ";NTC2=ERR");

    // Température ambiante
    if (spiOk)
        pos += snprintf(msg + pos, sizeof(msg) - pos, ";TMP126=%.2f", spiTemp);

    // INA237
    if (inaOk)
        pos += snprintf(msg + pos, sizeof(msg) - pos,
                        ";VBUS=%.3f;CURRENT=%.3f;POWER=%.3f",
                        busV, current, power);

    // Alarme
    pos += snprintf(msg + pos, sizeof(msg) - pos, ";ALARM=%d",
                    static_cast<int>(alarm));

    blemanager::notify(std::string(msg));
}

// ============================================================================
// ENVOI DES DERNIERS LOGS VIA BLE (EXF-19)
// ============================================================================
static void sendBleLastLogs(size_t count)
{
    if (s_log_count == 0)
    {
        blemanager::notify(std::string("LOGS=EMPTY"));
        return;
    }

    size_t n = (count > s_log_count) ? s_log_count : count;

    for (size_t i = 0; i < n; i++)
    {
        size_t idx = (s_log_head >= (i + 1))
                     ? s_log_head - i - 1
                     : LOG_BUFFER_SIZE - (i + 1 - s_log_head);

        const LogEntry &e = s_logs[idx];

        char buf[128];
        snprintf(buf, sizeof(buf), "LOG=%lu|%d|%.1f|%s",
                 (unsigned long)e.timestamp_s,
                 static_cast<int>(e.type),
                 e.value, e.message);

        blemanager::notify(std::string(buf));
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
