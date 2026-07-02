#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>
#include <driver/i2c.h>

#include "config/HardwareConfig.h"
#include "config/I2CDevicesConfig.h"

extern "C" void app_main(void)
{
    printf("\n=== ESP32 PICO-D4 HARDWARE TEST START ===\n");

    /*==============================================================
     * GPIO INIT
     *=============================================================*/
    gpio_set_direction(HardwareConfig::IHM::LED_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(HardwareConfig::IHM::BUZZER, GPIO_MODE_OUTPUT);

    /*==============================================================
     * ADC INIT (ESP-IDF v6 - ADC ONESHOT)
     *=============================================================*/
    adc_oneshot_unit_handle_t adc1_handle;

    adc_oneshot_unit_init_cfg_t init_cfg = {};
    init_cfg.unit_id = ADC_UNIT_1;
    init_cfg.ulp_mode = ADC_ULP_MODE_DISABLE;
    init_cfg.clk_src = ADC_RTC_CLK_SRC_DEFAULT;

    adc_oneshot_new_unit(&init_cfg, &adc1_handle);

    adc_oneshot_chan_cfg_t chan_cfg;
    chan_cfg.bitwidth = ADC_BITWIDTH_12;
    chan_cfg.atten = ADC_ATTEN_DB_12;

    // GPIO33 = ADC1_CHANNEL_5
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &chan_cfg);

    // GPIO34 = ADC1_CHANNEL_6
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &chan_cfg);

    /*==============================================================
     * I2C INIT (legacy driver temporairement OK)
     *=============================================================*/
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = HardwareConfig::I2C::SDA;
    conf.scl_io_num = HardwareConfig::I2C::SCL;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 400000;

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    while (true)
    {
        /*==========================================================
         * LED TEST
         *=========================================================*/
        gpio_set_level(HardwareConfig::IHM::LED_GREEN, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(HardwareConfig::IHM::LED_GREEN, 0);

        gpio_set_level(HardwareConfig::IHM::LED_RED, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(HardwareConfig::IHM::LED_RED, 0);

        /*==========================================================
         * BUZZER TEST
         *=========================================================*/
        gpio_set_level(HardwareConfig::IHM::BUZZER, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(HardwareConfig::IHM::BUZZER, 0);

        /*==========================================================
         * ADC TEST (NTC)
         *=========================================================*/
        int ntc1 = 0;
        int ntc2 = 0;

        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &ntc1); // GPIO34
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_5, &ntc2); // GPIO33

        printf("NTC1(GPIO34) = %d | NTC2(GPIO33) = %d\n", ntc1, ntc2);

        /*==========================================================
         * I2C TEST (INA237)
         *=========================================================*/
        uint8_t addr = I2CDevices::Power::INA237_ADRESS;

        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK)
        {
            printf("INA237 detected at 0x%02X ✅\n", addr);
        }
        else
        {
            printf("INA237 NOT FOUND ❌ (0x%02X)\n", addr);
        }

        printf("System OK\n\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}