#pragma once

#include <cstdint>

namespace INA237Defs
{

    constexpr uint8_t ADDRESS = 0x40;

    enum class Register : uint8_t
    {
        CONFIG = 0x00,
        ADC_CONFIG = 0x01,
        SHUNT_CAL = 0x02,
        SHUNT_TEMPCO = 0x03,
        VSHUNT = 0x04,
        VBUS = 0x05,
        DIETEMP = 0x06,
        CURRENT = 0x07,
        POWER = 0x08,
        DIAG_ALRT = 0x09,
        SOVL = 0x0A,
        SUVL = 0x0B,
        BOVL = 0x0C,
        BUVL = 0x0D,
        TEMP_LIMIT = 0x0E,
        PWR_LIMIT = 0x0F,
        MANUFACTURER_ID = 0x3E,
        DEVICE_ID = 0x3F
    };

    constexpr uint16_t MANUFACTURER_ID_VALUE = 0x5449;

} // namespace INA237Defs