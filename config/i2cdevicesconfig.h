#pragma once

#include <cstdint>

namespace I2CDevices
{
    namespace Power
    {
        // INA237AIDGST (A1=GND  A2=GND)
        constexpr uint8_t INA237_ADRESS = 0x40;
    }

    namespace Grove
    {
        constexpr uint8_t PORT_1 = 0x20;
        constexpr uint8_t PORT_2 = 0x21;
    }

    namespace Mikrobus
    {
        constexpr uint8_t SLOT_1 = 0x48;
    }
}