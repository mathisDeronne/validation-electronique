#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"

class SPIManager
{
public:
    SPIManager();

    esp_err_t init();

    // Lecture température TMP126 en °C
    esp_err_t readTemperature(float &temperature);

    // Lecture brute du registre température 0x00
    esp_err_t readRaw(uint16_t &rx);

    // Debug : lecture du Device ID, doit normalement retourner 0x2126
    esp_err_t readDeviceId(uint16_t &deviceId);

private:
    spi_device_handle_t spi;

    // GPIO SPI ESP32
    // TMP126 : SIO est bidirectionnel.
    // Câblage recommandé : MOSI -> résistance 1k à 10k -> SIO, et MISO -> SIO directement.
    static constexpr gpio_num_t PIN_MOSI = GPIO_NUM_23;
    static constexpr gpio_num_t PIN_MISO = GPIO_NUM_19;
    static constexpr gpio_num_t PIN_CLK  = GPIO_NUM_18;
    static constexpr gpio_num_t PIN_CS   = GPIO_NUM_5;

    static constexpr spi_host_device_t SPI_HOST = SPI2_HOST;

    // Mets true uniquement si ta carte relie SIO seulement au MOSI,
    // sans MISO séparé connecté à SIO.
    static constexpr bool USE_3WIRE_MODE = false;

    // Registres TMP126 utiles
    static constexpr uint8_t TMP126_REG_TEMPERATURE = 0x00;
    static constexpr uint8_t TMP126_REG_DEVICE_ID   = 0x0C;

    esp_err_t readRegister(uint8_t reg, uint16_t &value);
};
