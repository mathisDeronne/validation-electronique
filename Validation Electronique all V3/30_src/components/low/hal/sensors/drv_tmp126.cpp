// ============================================================================
// FILE: components/low/hal/sensors/drv_tmp126.cpp
// LAYER: LOW (HAL - capteur température SPI)
// PURPOSE: Implémentation du driver TMP126DCKR
//
// Conversion température (datasheet TMP126) :
//   Le registre température contient une valeur 14 bits signée.
//   Résolution = 0.03125°C par LSB
//   temp_c = raw_14bits × 0.03125
// ============================================================================

#include "drv_tmp126.hpp"
#include "HardwareConfig.hpp"

// Sur cible ESP32 réelle :
// #include "driver/spi_master.h"
// #include "driver/gpio.h"

namespace low {

DrvTmp126::DrvTmp126()
    : m_initialized(false)
{
}

common::Status DrvTmp126::init()
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // spi_bus_config_t bus_cfg = {
    //     .mosi_io_num   = config::hardware::SPI_TMP126_MOSI,  // GPIO23
    //     .miso_io_num   = config::hardware::SPI_TMP126_MISO,  // GPIO19
    //     .sclk_io_num   = config::hardware::SPI_TMP126_SCK,   // GPIO18
    // };
    // spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_DISABLED);
    //
    // spi_device_interface_config_t dev_cfg = {
    //     .clock_speed_hz = 1000000,  // 1 MHz
    //     .spics_io_num   = config::hardware::SPI_TMP126_CS,   // GPIO5
    //     .queue_size     = 1,
    // };
    // spi_bus_add_device(SPI2_HOST, &dev_cfg, &m_spi_handle);

    m_initialized = true;

    return result;
}

common::float32_t DrvTmp126::readTemperatureC()
{
    common::float32_t temp_c = 25.0F;

    if (m_initialized)
    {
        uint16_t raw = 0U;
        const common::Status result = spiRead(REG_TEMP, raw);

        if (result == common::Status::OK)
        {
            // Le registre contient 14 bits signés (bits 15:2)
            // On décale de 2 bits à droite pour obtenir la valeur 14 bits
            const int16_t raw_signed = static_cast<int16_t>(raw) >> 2;

            // Résolution = 0.03125°C par LSB (datasheet TMP126)
            constexpr common::float32_t LSB_RESOLUTION = 0.03125F;
            temp_c = static_cast<common::float32_t>(raw_signed) * LSB_RESOLUTION;
        }
    }

    return temp_c;
}

bool DrvTmp126::selfTest()
{
    bool present = false;

    if (m_initialized)
    {
        uint16_t device_id = 0U;
        const common::Status result = spiRead(REG_DEVICE_ID, device_id);

        if (result == common::Status::OK)
        {
            // TMP126 device ID attendu (datasheet)
            present = (device_id != 0x0000U) && (device_id != 0xFFFFU);
        }
    }

    return present;
}

// ============================================================================
// SPI BAS NIVEAU — appelle ESP-IDF directement
// ============================================================================

common::Status DrvTmp126::spiRead(uint8_t reg, uint16_t& out_value)
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // spi_transaction_t t = {};
    // t.length = 24;  // 8 bits adresse + 16 bits données
    // t.tx_data[0] = reg | 0x80U;  // Bit R/W = 1 (lecture)
    // t.flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;
    // esp_err_t err = spi_device_transmit(m_spi_handle, &t);
    // out_value = (t.rx_data[1] << 8) | t.rx_data[2];
    // result = (err == ESP_OK) ? OK : ERROR_I2C;

    (void)reg;
    out_value = 0x0320U;  // Mock : 800 × 0.03125 = 25.0°C

    return result;
}

common::Status DrvTmp126::spiWrite(uint8_t reg, uint16_t value)
{
    common::Status result = common::Status::OK;

    // --- Cible réelle ESP-IDF ---
    // spi_transaction_t t = {};
    // t.length = 24;
    // t.tx_data[0] = reg & 0x7FU;  // Bit R/W = 0 (écriture)
    // t.tx_data[1] = (uint8_t)(value >> 8);
    // t.tx_data[2] = (uint8_t)(value & 0xFFU);
    // result = (spi_device_transmit(...) == ESP_OK) ? OK : ERROR;

    (void)reg;
    (void)value;

    return result;
}

} // namespace low
