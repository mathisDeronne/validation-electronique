#include <stdio.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_err.h>
#include "esp_log.h"

#include "HardwareConfig.hpp"
#include "TempSensor.hpp"
#include "buzzermelody.hpp"
#include "blemanager.hpp"
#include "SPIManager.hpp"
#include "I2CBus.hpp"
#include "INA237.hpp"

static const char *TAG_MAIN = "MAIN";
static const char *TAG_ADC  = "ADC";
static const char *TAG_TMP  = "TMP126";

extern "C" void app_main(void)
{
    printf("\n=== ESP32 PICO D4 - BLE CONTROL SYSTEM ===\n");

    //--------------------------------------------------------------
    // GPIO INIT : LED + BUZZER
    //--------------------------------------------------------------
    gpio_set_direction(HardwareConfig::IHM::LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::BUZZER, GPIO_MODE_OUTPUT);

    gpio_set_level(HardwareConfig::IHM::LED_GREEN, 0);
    gpio_set_level(HardwareConfig::IHM::LED_RED, 0);
    gpio_set_level(HardwareConfig::IHM::BUZZER, 0);

    //--------------------------------------------------------------
    // BLE INIT
    //--------------------------------------------------------------
    // Attention : le nom doit être exactement le même que dans le HTML.
    // Dans ton HTML, le filtre cherche "Groupe 5".
    if (!blemanager::init("Groupe 5"))
    {
        printf("BLE init FAILED\n");
        return;
    }

    blemanager::start();
    printf("BLE started - waiting commands...\n");

    //--------------------------------------------------------------
    // ADC INIT : NTC sur GPIO25 et GPIO26
    //--------------------------------------------------------------
    adc_oneshot_unit_handle_t adc_handle = nullptr;
    bool adc_ok = true;

    adc_oneshot_unit_init_cfg_t init_cfg = {};
    init_cfg.unit_id = ADC_UNIT_2;              // GPIO25/GPIO26 = ADC2 sur ESP32
    init_cfg.ulp_mode = ADC_ULP_MODE_DISABLE;
    init_cfg.clk_src = ADC_RTC_CLK_SRC_DEFAULT;

    esp_err_t adc_err = adc_oneshot_new_unit(&init_cfg, &adc_handle);

    if (adc_err != ESP_OK)
    {
        adc_ok = false;
        ESP_LOGE(TAG_ADC, "Erreur init ADC: %s", esp_err_to_name(adc_err));
    }
    else
    {
        adc_oneshot_chan_cfg_t chan_cfg = {};
        chan_cfg.bitwidth = ADC_BITWIDTH_12;
        chan_cfg.atten = ADC_ATTEN_DB_12;

        // NTC1 : GPIO25 = ADC2_CHANNEL_8
        adc_err = adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_8, &chan_cfg);
        if (adc_err != ESP_OK)
        {
            adc_ok = false;
            ESP_LOGE(TAG_ADC, "Erreur config ADC_CHANNEL_8: %s", esp_err_to_name(adc_err));
        }

        // NTC2 : GPIO26 = ADC2_CHANNEL_9
        adc_err = adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_9, &chan_cfg);
        if (adc_err != ESP_OK)
        {
            adc_ok = false;
            ESP_LOGE(TAG_ADC, "Erreur config ADC_CHANNEL_9: %s", esp_err_to_name(adc_err));
        }

        if (adc_ok)
        {
            ESP_LOGI(TAG_ADC, "ADC initialise correctement");
        }
    }

    //--------------------------------------------------------------
    // TMP126 SPI INIT
    //--------------------------------------------------------------
    SPIManager spi;
    bool spi_ok = false;

    if (spi.init() != ESP_OK)
    {
        ESP_LOGE(TAG_TMP, "Erreur initialisation SPI");
        spi_ok = false;
    }
    else
    {
        ESP_LOGI(TAG_TMP, "SPI initialise");
        spi_ok = true;
    }

    //--------------------------------------------------------------
    // INA237 I2C INIT : mesure tension / courant / puissance
    //--------------------------------------------------------------
    I2CBus bus;
    bool ina_ok = false;

    bus.begin(21, 22, 400000);

    INA237 ina(bus);

    if (!ina.begin())
    {
        ESP_LOGE(TAG_MAIN, "INA237 non detecte");
        ina_ok = false;
    }
    else
    {
        ESP_LOGI(TAG_MAIN, "INA237 initialise");
        ina.setCalibration(10000, 2000); // Calibration provisoire depuis mainINA.cpp
        ina_ok = true;
    }

    //--------------------------------------------------------------
    // VARIABLES
    //--------------------------------------------------------------
    int adc1 = 0;
    int adc2 = 0;

    float temp1 = -999.0f;
    float temp2 = -999.0f;
    float spiTemp = -999.0f;
    float inaBusVoltage = -999.0f;
    float inaCurrent = -999.0f;
    float inaPower = -999.0f;

    uint32_t loopCounter = 0;

    //--------------------------------------------------------------
    // LOOP PRINCIPALE
    //--------------------------------------------------------------
    while (true)
    {
        //----------------------------------------------------------
        // BLE RECEIVER : commandes reçues depuis le HTML
        //----------------------------------------------------------
        if (blemanager::available())
        {
            std::string cmd = blemanager::read();
            printf("CMD BLE => %s\n", cmd.c_str());

            if (cmd == "LED_GREEN_ON")
            {
                gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);
            }
            else if (cmd == "LED_GREEN_OFF")
            {
                gpio_set_level(HardwareConfig::IHM::LED_GREEN, 0);
            }
            else if (cmd == "LED_RED_ON")
            {
                gpio_set_level(HardwareConfig::IHM::LED_RED, 1);
            }
            else if (cmd == "LED_RED_OFF")
            {
                gpio_set_level(HardwareConfig::IHM::LED_RED, 0);
            }
            else if (cmd == "BUZZER_ON")
            {
                gpio_set_level(HardwareConfig::IHM::BUZZER, 1);
            }
            else if (cmd == "BUZZER_OFF")
            {
                gpio_set_level(HardwareConfig::IHM::BUZZER, 0);
            }
            else if (cmd == "ALARM_1")
            {
                playAlarm(AlarmSound::TripleBeep);
            }
            else if (cmd == "ALARM_2")
            {
                playAlarm(AlarmSound::Siren);
            }
            else if (cmd == "ALARM_3")
            {
                playAlarm(AlarmSound::Warning);
            }
            else if (cmd == "ALARM_4")
            {
                playAlarm(AlarmSound::Emergency);
            }
            else
            {
                ESP_LOGW(TAG_MAIN, "Commande inconnue: %s", cmd.c_str());
            }
        }

        //----------------------------------------------------------
        // ADC READ : sans ESP_ERROR_CHECK pour éviter les reboots
        //----------------------------------------------------------
        if (adc_ok && adc_handle != nullptr)
        {
            esp_err_t err_adc1 = adc_oneshot_read(adc_handle, ADC_CHANNEL_8, &adc1);
            esp_err_t err_adc2 = adc_oneshot_read(adc_handle, ADC_CHANNEL_9, &adc2);

            if (err_adc1 == ESP_OK)
            {
                temp1 = adcToTemperature(adc1);
            }
            else
            {
                temp1 = -999.0f;
                ESP_LOGE(TAG_ADC, "Erreur lecture NTC1: %s", esp_err_to_name(err_adc1));
            }

            if (err_adc2 == ESP_OK)
            {
                temp2 = adcToTemperature(adc2);
            }
            else
            {
                temp2 = -999.0f;
                ESP_LOGE(TAG_ADC, "Erreur lecture NTC2: %s", esp_err_to_name(err_adc2));
            }
        }
        else
        {
            adc1 = 0;
            adc2 = 0;
            temp1 = -999.0f;
            temp2 = -999.0f;
        }

        //----------------------------------------------------------
        // TMP126 SPI READ : pas de reboot si erreur
        //----------------------------------------------------------
        bool tmp126Ok = false;

        if (spi_ok)
        {
            tmp126Ok = (spi.readTemperature(spiTemp) == ESP_OK);

            if (!tmp126Ok)
            {
                spiTemp = -999.0f;
            }
        }

        //----------------------------------------------------------
        // INA237 I2C READ
        //----------------------------------------------------------
        if (ina_ok)
        {
            inaBusVoltage = ina.readBusVoltage();
            inaCurrent = ina.readCurrent();
            inaPower = ina.readPower();
        }
        else
        {
            inaBusVoltage = -999.0f;
            inaCurrent = -999.0f;
            inaPower = -999.0f;
        }

        //----------------------------------------------------------
        // AFFICHAGE SERIE
        //----------------------------------------------------------
        printf("\n-----------------------------\n");

        if (temp1 > -900.0f)
        {
            printf("NTC1 (GPIO25) : %4d -> %.2f C\n", adc1, temp1);
        }
        else
        {
            printf("NTC1 (GPIO25) : Erreur\n");
        }

        if (temp2 > -900.0f)
        {
            printf("NTC2 (GPIO26) : %4d -> %.2f C\n", adc2, temp2);
        }
        else
        {
            printf("NTC2 (GPIO26) : Erreur\n");
        }

        if (tmp126Ok)
        {
            printf("TMP126 (SPI)  : %.2f C\n", spiTemp);
        }
        else
        {
            printf("TMP126 (SPI)  : Erreur\n");
        }

        if (ina_ok)
        {
            printf("INA237 Bus    : %.3f V\n", inaBusVoltage);
            printf("INA237 Current: %.3f A\n", inaCurrent);
            printf("INA237 Power  : %.3f W\n", inaPower);
        }
        else
        {
            printf("INA237        : Erreur\n");
        }

        printf("BLE connected : %s\n", blemanager::isConnected() ? "YES" : "NO");
        printf("-----------------------------\n");

        //----------------------------------------------------------
        // BLE SEND DATA
        // Envoi seulement toutes les 5 boucles environ = 1 seconde
        //----------------------------------------------------------
        if (blemanager::isConnected() && (loopCounter % 5 == 0))
        {
            char msg[256];
            int pos = 0;

            // Températures NTC
            pos += snprintf(msg + pos, sizeof(msg) - pos,
                            "NTC1=%.2f;NTC2=%.2f",
                            temp1, temp2);

            // TMP126 SPI
            if (tmp126Ok)
            {
                pos += snprintf(msg + pos, sizeof(msg) - pos,
                                ";TMP126=%.2f", spiTemp);
            }

            // INA237 I2C
            if (ina_ok)
            {
                pos += snprintf(msg + pos, sizeof(msg) - pos,
                                ";VBUS=%.3f;CURRENT=%.3f;POWER=%.3f",
                                inaBusVoltage, inaCurrent, inaPower);
            }

            blemanager::notify(std::string(msg));
        }

        loopCounter++;

        //----------------------------------------------------------
        // LOOP SPEED
        //----------------------------------------------------------
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
