#include "I2CBus.hpp"
#include "esp_log.h"

static const char *TAG = "I2CBus";

I2CBus::I2CBus(i2c_port_t port) : _port(port) {}

void I2CBus::begin(int sda, int scl, uint32_t freq_hz)
{

    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)sda;
    conf.scl_io_num = (gpio_num_t)scl;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = freq_hz;

    i2c_param_config(_port, &conf);
    i2c_driver_install(_port, conf.mode, 0, 0, 0);

    ESP_LOGI(TAG, "I2C initialisé");
}

bool I2CBus::devicePresent(uint8_t address)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(_port, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);

    return err == ESP_OK;
}

I2CStatus I2CBus::write16(uint8_t address, uint8_t reg, uint16_t data)
{

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, (data >> 8) & 0xFF, true);
    i2c_master_write_byte(cmd, data & 0xFF, true);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    return (err == ESP_OK) ? I2CStatus::OK : I2CStatus::ERROR;
}

I2CStatus I2CBus::read16(uint8_t address, uint8_t reg, uint16_t &data)
{

    uint8_t buffer[2];

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);

    i2c_master_read(cmd, buffer, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    if (err != ESP_OK)
        return I2CStatus::ERROR;

    data = (buffer[0] << 8) | buffer[1];
    return I2CStatus::OK;
}