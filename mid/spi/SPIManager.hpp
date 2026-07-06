#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"

class SPIManager
{
public:
    SPIManager();

    esp_err_t init();

    esp_err_t readTemperature(float &temperature);

    // 🔥 DEBUG : lecture brute (très utile pour ton cas)
    esp_err_t readRaw(uint16_t &rx);

private:
    spi_device_handle_t spi;

    static constexpr gpio_num_t PIN_MOSI = GPIO_NUM_23;
    static constexpr gpio_num_t PIN_MISO = GPIO_NUM_19;
    static constexpr gpio_num_t PIN_CLK = GPIO_NUM_18;
    static constexpr gpio_num_t PIN_CS = GPIO_NUM_5;

    esp_err_t transfer(uint16_t tx, uint16_t &rx);
};