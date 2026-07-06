#include "SPIManager.hpp"

#include <string.h>
#include <stdio.h>

SPIManager::SPIManager()
{
    spi = nullptr;
}

esp_err_t SPIManager::init()
{
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = PIN_MOSI;
    buscfg.miso_io_num = USE_3WIRE_MODE ? -1 : PIN_MISO;
    buscfg.sclk_io_num = PIN_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 4;

    esp_err_t err = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_DISABLED);

    // Si le bus SPI est déjà initialisé ailleurs, on continue.
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        printf("SPI bus init error: %s\n", esp_err_to_name(err));
        return err;
    }

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 100000;      // Lent pour debug
    devcfg.mode = 0;                     // Si besoin, tester ensuite 1 puis 3
    devcfg.spics_io_num = PIN_CS;
    devcfg.queue_size = 1;

    // TMP126 : le capteur reçoit une commande 16 bits, puis répond sur 16 bits.
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;

    // Option uniquement si SIO est câblé en vraie ligne unique 3 fils.
    if (USE_3WIRE_MODE)
    {
        devcfg.flags |= SPI_DEVICE_3WIRE;
    }

    err = spi_bus_add_device(SPI_HOST, &devcfg, &spi);

    if (err != ESP_OK)
    {
        printf("SPI add device error: %s\n", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t SPIManager::readRegister(uint8_t reg, uint16_t &value)
{
    if (spi == nullptr)
    {
        return ESP_ERR_INVALID_STATE;
    }

    // Commande TMP126 :
    // bit 8 = 1 pour READ
    // bits [7:0] = adresse du registre
    // Exemple : reg 0x00 -> commande 0x0100
    // Exemple : reg 0x0C -> commande 0x010C
    const uint16_t cmd = 0x0100 | reg;

    // IMPORTANT : ESP32 est little-endian.
    // On construit donc manuellement les octets en MSB first.
    uint8_t tx_buf[2] = {
        static_cast<uint8_t>((cmd >> 8) & 0xFF),
        static_cast<uint8_t>(cmd & 0xFF)
    };

    uint8_t rx_buf[2] = {0x00, 0x00};

    spi_transaction_t t = {};
    memset(&t, 0, sizeof(t));

    t.length = 16;        // 16 bits de commande envoyés
    t.rxlength = 16;      // puis 16 bits reçus
    t.tx_buffer = tx_buf;
    t.rx_buffer = rx_buf;

    esp_err_t err = spi_device_transmit(spi, &t);

    if (err != ESP_OK)
    {
        printf("TMP126 read reg 0x%02X error: %s\n", reg, esp_err_to_name(err));
        return err;
    }

    // Réception MSB first.
    value = (static_cast<uint16_t>(rx_buf[0]) << 8) | rx_buf[1];

    printf("TMP126 reg 0x%02X = 0x%04X\n", reg, value);

    return ESP_OK;
}

esp_err_t SPIManager::readRaw(uint16_t &rx)
{
    return readRegister(TMP126_REG_TEMPERATURE, rx);
}

esp_err_t SPIManager::readDeviceId(uint16_t &deviceId)
{
    return readRegister(TMP126_REG_DEVICE_ID, deviceId);
}

esp_err_t SPIManager::readTemperature(float &temperature)
{
    uint16_t raw = 0;

    esp_err_t err = readRaw(raw);
    if (err != ESP_OK)
    {
        return err;
    }

    // TMP126 : température sur 14 bits, bits [15:2]
    int16_t value = static_cast<int16_t>(raw >> 2);

    // Extension de signe 14 bits.
    if (value & 0x2000)
    {
        value |= 0xC000;
    }

    // Résolution TMP126 : 0.03125 °C / LSB
    temperature = value * 0.03125f;

    printf("TMP126 raw = 0x%04X | temperature = %.2f C\n", raw, temperature);

    return ESP_OK;
}
