#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>

#include "HardwareConfig.hpp"
#include "TempSensor.hpp"
#include "buzzermelody.hpp"

extern "C" void app_main(void)
{
    printf("\n=== ESP32 HARDWARE TEST ===\n");

    //--------------------------------------------------------------
    // GPIO INIT
    //--------------------------------------------------------------
    gpio_set_direction(HardwareConfig::IHM::LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::BUZZER, GPIO_MODE_OUTPUT);

    //--------------------------------------------------------------
    // ADC  INIT
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

    // GPIO33 = ADC1_CHANNEL_5
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle,
                                               ADC_CHANNEL_5,
                                               &chan_cfg));

    // GPIO34 = ADC1_CHANNEL_6
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle,
                                               ADC_CHANNEL_6,
                                               &chan_cfg));

    //--------------------------------------------------------------
    // LOOP
    //--------------------------------------------------------------
    while (true)
    {
        //=============================
        // LED TEST
        //=============================
        gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(HardwareConfig::IHM::LED_GREEN, 0);

        gpio_set_level(HardwareConfig::IHM::LED_RED, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(HardwareConfig::IHM::LED_RED, 0);

        //=============================
        // TEMPERATURE
        //=============================
        int adc1 = 0;
        int adc2 = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle,
                                         ADC_CHANNEL_5,
                                         &adc1));

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle,
                                         ADC_CHANNEL_6,
                                         &adc2));

        float temp1 = adcToTemperature(adc1);
        float temp2 = adcToTemperature(adc2);

        printf("\n-----------------------------\n");
        printf("NTC1 : %4d -> %.2f °C\n", adc1, temp1);
        printf("NTC2 : %4d -> %.2f °C\n", adc2, temp2);
        printf("-----------------------------\n");

        //=============================
        // TEST DES 4 MELODIES
        //=============================
        /*
        printf("Melodie 1 : Triple Beep\n");
        playAlarm(AlarmSound::TripleBeep);
        vTaskDelay(pdMS_TO_TICKS(1000));

        printf("Melodie 2 : Siren\n");
        playAlarm(AlarmSound::Siren);
        vTaskDelay(pdMS_TO_TICKS(1000));

        printf("Melodie 3 : Warning\n");
        playAlarm(AlarmSound::Warning);
        vTaskDelay(pdMS_TO_TICKS(1000));

        printf("Melodie 4 : Emergency\n");
        playAlarm(AlarmSound::Emergency);
        vTaskDelay(pdMS_TO_TICKS(2000));
        */
    }
}