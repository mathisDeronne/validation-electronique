#include <stdio.h>
#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>

#include "HardwareConfig.hpp"
#include "TempSensor.hpp"
#include "buzzermelody.hpp"
#include "blemanager.hpp"

extern "C" void app_main(void)
{
    printf("\n=== ESP32 PICO D4 - BLE CONTROL SYSTEM ===\n");

    //--------------------------------------------------------------
    // GPIO INIT
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
    if (!blemanager::init("ESP32-PICO-D4"))
    {
        printf("BLE init FAILED\n");
        return;
    }

    blemanager::start();
    printf("BLE started - waiting commands...\n");

    //--------------------------------------------------------------
    // ADC INIT
    //--------------------------------------------------------------
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_cfg = {};
    init_cfg.unit_id = ADC_UNIT_1;
    init_cfg.ulp_mode = ADC_ULP_MODE_DISABLE;
    init_cfg.clk_src = ADC_RTC_CLK_SRC_DEFAULT;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {};
    chan_cfg.bitwidth = ADC_BITWIDTH_12;
    chan_cfg.atten = ADC_ATTEN_DB_12;

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_5, &chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &chan_cfg));

    //--------------------------------------------------------------
    // LOOP
    //--------------------------------------------------------------
    while (true)
    {
        //----------------------------------------------------------
        // BLE RECEIVER
        //----------------------------------------------------------
        if (blemanager::available())
        {
            std::string cmd = blemanager::read();
            printf("CMD BLE => %s\n", cmd.c_str());

            //========================
            // LED VERTE
            //========================
            if (cmd == "LED_GREEN_ON")
                gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);

            else if (cmd == "LED_GREEN_OFF")
                gpio_set_level(HardwareConfig::IHM::LED_GREEN, 0);

            //========================
            // LED ROUGE
            //========================
            else if (cmd == "LED_RED_ON")
                gpio_set_level(HardwareConfig::IHM::LED_RED, 1);

            else if (cmd == "LED_RED_OFF")
                gpio_set_level(HardwareConfig::IHM::LED_RED, 0);

            //========================
            // BUZZER
            //========================
            else if (cmd == "BUZZER_ON")
                gpio_set_level(HardwareConfig::IHM::BUZZER, 1);

            else if (cmd == "BUZZER_OFF")
                gpio_set_level(HardwareConfig::IHM::BUZZER, 0);

            //========================
            // MELODIES
            //========================
            else if (cmd == "ALARM_1")
                playAlarm(AlarmSound::TripleBeep);

            else if (cmd == "ALARM_2")
                playAlarm(AlarmSound::Siren);

            else if (cmd == "ALARM_3")
                playAlarm(AlarmSound::Warning);

            else if (cmd == "ALARM_4")
                playAlarm(AlarmSound::Emergency);
        }

        //----------------------------------------------------------
        // ADC READ (TEMP)
        //----------------------------------------------------------
        int adc1 = 0;
        int adc2 = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_5, &adc1));
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &adc2));

        float temp1 = adcToTemperature(adc1);
        float temp2 = adcToTemperature(adc2);

        printf("T1: %.2f°C | T2: %.2f°C\n", temp1, temp2);

        //----------------------------------------------------------
        // BLE SEND DATA (optionnel)
        //----------------------------------------------------------
        if (blemanager::isConnected())
        {
            char msg[80];
            snprintf(msg, sizeof(msg),
                     "TEMP1=%.2f;TEMP2=%.2f", temp1, temp2);

            blemanager::notify(std::string(msg));
        }

        //----------------------------------------------------------
        // LOOP SPEED
        //----------------------------------------------------------
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}