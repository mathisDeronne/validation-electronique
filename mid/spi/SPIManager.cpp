#include "SPIManager.hpp"
#include <string.h>

SPIManager::SPIManager()
{
    spi = nullptr;
}

esp_err_t SPIManager::init()
{
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = PIN_MOSI;
    buscfg.miso_io_num = PIN_MISO;
    buscfg.sclk_io_num = PIN_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    esp_err_t err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED);
    if (err != ESP_OK)
        return err;

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 100000; // lent pour debug
    devcfg.mode = 0;                // à tester si besoin 0→3
    devcfg.spics_io_num = PIN_CS;
    devcfg.queue_size = 1;
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;

    return spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
}

esp_err_t SPIManager::transfer(uint16_t tx, uint16_t &rx)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 16;
    t.rxlength = 16;

    t.tx_buffer = &tx;
    t.rx_buffer = &rx;

    return spi_device_transmit(spi, &t);
}

esp_err_t SPIManager::readRaw(uint16_t &rx)
{
    uint16_t tx = 0x0000; // commande neutre pour debug
    return transfer(tx, rx);
}

esp_err_t SPIManager::readTemperature(float & temperature)
{
    uint16_t rx = 0;

    esp_err_t err = readRaw(rx);
    if (err != ESP_OK)
        return err;

    // ⚠️ conversion générique (TMP126 réel peut varier)
    int16_t value = (int16_t)(rx >> 2);

    if (value & 0x2000)
        value |= 0xC000;

    temperature = value * 0.03125f;

    return ESP_OK;
}