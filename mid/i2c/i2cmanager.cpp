#include "I2CManager.hpp"

#include <stdio.h>

#include "HardwareConfig.hpp"

bool I2CManager::initialized = false;

//--------------------------------------------------------------
// Initialisation
//--------------------------------------------------------------

esp_err_t I2CManager::init()
{
    if (initialized)
    {
        return ESP_OK;
    }

    i2c_config_t config = {};
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = HardwareConfig::I2C::SDA;
    config.scl_io_num = HardwareConfig::I2C::SCL;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = 100000;

    ESP_ERROR_CHECK(i2c_param_config(PORT, &config));
    ESP_ERROR_CHECK(i2c_driver_install(PORT,
                                       config.mode,
                                       0,
                                       0,
                                       0));

    initialized = true;

    return ESP_OK;
}

//--------------------------------------------------------------
// Désinitialisation
//--------------------------------------------------------------

void I2CManager::deinit()
{
    if (!initialized)
    {
        return;
    }

    i2c_driver_delete(PORT);

    initialized = false;
}

//--------------------------------------------------------------
// Vérifie la présence d'un périphérique
//--------------------------------------------------------------

bool I2CManager::devicePresent(uint8_t address)
{
    if (!initialized)
    {
        return false;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,
                          (address << 1) | I2C_MASTER_WRITE,
                          true);
    i2c_master_stop(cmd);

    esp_err_t ret =
        i2c_master_cmd_begin(PORT,
                             cmd,
                             TIMEOUT);

    i2c_cmd_link_delete(cmd);

    return (ret == ESP_OK);
}

//--------------------------------------------------------------
// Scan du bus
//--------------------------------------------------------------

uint8_t I2CManager::scan(bool printResult)
{
    uint8_t found = 0;

    if (printResult)
    {
        printf("\n====== I2C Scan ======\n");
    }

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        if (devicePresent(addr))
        {
            found++;

            if (printResult)
            {
                printf("Device found : 0x%02X\n", addr);
            }
        }
    }

    if (printResult)
    {
        if (found == 0)
        {
            printf("No device found\n");
        }

        printf("Total devices : %d\n", found);
        printf("======================\n");
    }

    return found;
}

//--------------------------------------------------------------
// Write
//--------------------------------------------------------------

esp_err_t I2CManager::write(
    uint8_t address,
    const uint8_t *data,
    size_t length)
{
    if (!initialized)
    {
        return ESP_FAIL;
    }

    return i2c_master_write_to_device(
        PORT,
        address,
        data,
        length,
        TIMEOUT);
}

//--------------------------------------------------------------
// Read
//--------------------------------------------------------------

esp_err_t I2CManager::read(
    uint8_t address,
    uint8_t *data,
    size_t length)
{
    if (!initialized)
    {
        return ESP_FAIL;
    }

    return i2c_master_read_from_device(
        PORT,
        address,
        data,
        length,
        TIMEOUT);
}

//--------------------------------------------------------------
// Write Register
//--------------------------------------------------------------

esp_err_t I2CManager::writeRegister(
    uint8_t address,
    uint8_t reg,
    uint8_t value)
{
    uint8_t buffer[2];

    buffer[0] = reg;
    buffer[1] = value;

    return write(address,
                 buffer,
                 sizeof(buffer));
}

//--------------------------------------------------------------
// Read Register
//--------------------------------------------------------------

esp_err_t I2CManager::readRegister(
    uint8_t address,
    uint8_t reg,
    uint8_t *value)
{
    if (!initialized)
    {
        return ESP_FAIL;
    }

    esp_err_t ret =
        i2c_master_write_read_device(
            PORT,
            address,
            &reg,
            1,
            value,
            1,
            TIMEOUT);

    return ret;
}