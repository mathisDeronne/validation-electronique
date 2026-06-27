#pragma once

#include <driver/gpio.h>

namespace HardwareConfig
{
    /* GPIO IHM Output */ 
    namespace IHM
    {
        constexpr gpio_num_t BUZZER     = GPIO_NUM_13;  // Buzzer_PWM
        constexpr gpio_num_t LED_GREEN  = GPIO_NUM_14;  // CMD_LED_GREEN
        constexpr gpio_num_t LED_RED    = GPIO_NUM_15;  // CMD_LED_RED
    }

    /* GPIO I2C Bus */
    namespace I2C
    {
        constexpr gpio_num_t SDA    = GPIO_NUM_21;
        constexpr gpio_num_t SCL    = GPIO_NUM_22;
    }

    /* GPIO SPI Bus */
    namespace SPI
    {
        constexpr gpio_num_t MISO   = GPIO_NUM_19;
        constexpr gpio_num_t MOSI   = GPIO_NUM_23;
        constexpr gpio_num_t SCK    = GPIO_NUM_18;
        constexpr gpio_num_t CS     = GPIO_NUM_5;
    }

    /* GPIO Mikrobus */
    namespace Mikrobus
    {
        constexpr gpio_num_t IO12   = GPIO_NUM_12;  // PWM
        constexpr gpio_num_t IO32   = GPIO_NUM_32;  // AN
        constexpr gpio_num_t IO33   = GPIO_NUM_33;  // RST
        constexpr gpio_num_t IO34   = GPIO_NUM_34;  // INT (input only)
    }

    /* GPIO PSRAM */
    namespace PSRAM
    {
        // GPIO 17 = SO/SIO1
        constexpr gpio_num_t IO17   = GPIO_NUM_17;
    }

    /* GPIO System */
    namespace System
    {
        // GPIO 2 connecté à GND (non utiisé) -> pour le changement de mode au Boot

        constexpr gpio_num_t BOOT_MODE  = GPIO_NUM_0;   // Bouton Boot 

        // Bouton Rest (EN PIN - non contrôlable en  software)

        // U0TXD GPIO 1
        // U0RXD GPIO 3

        // LNA_IN (antaine RF) -> Géré par le module ESP32 (WIFI/BLE)
    }
}