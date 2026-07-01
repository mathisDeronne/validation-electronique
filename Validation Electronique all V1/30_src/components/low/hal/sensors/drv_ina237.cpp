// ============================================================================
// FILE: components/low/hal/sensors/drv_ina237.cpp
// LAYER: LOW (HAL - capteur courant)
// PURPOSE: Implémentation du driver INA237
// ============================================================================

#include "drv_ina237.hpp"
#include "drv_ina237_convert.hpp"
#include "SystemConfig.hpp"

namespace low {

DrvIna237::DrvIna237(DrvI2c& i2c_bus, uint8_t i2c_addr)
    : m_i2c(i2c_bus)
    , m_addr(i2c_addr)
    , m_currentLsbUa(config::system::INA237_CURRENT_LSB_UA)
{
}

common::Status DrvIna237::configure()
{
    common::Status result = common::Status::OK;

    result = m_i2c.writeRegister(m_addr, REG_CONFIG, 0x4127U);

    return result;
}

common::Status DrvIna237::readCurrentMa(int32_t& out_current_ma)
{
    common::Status result = common::Status::OK;
    uint16_t raw = 0U;

    result = m_i2c.readRegister(m_addr, REG_CURRENT, raw);

    if (result == common::Status::OK)
    {
        const int16_t raw_signed = static_cast<int16_t>(raw);
        out_current_ma = ina237::convertCurrentRawToMa(raw_signed, m_currentLsbUa);
    }
    else
    {
        out_current_ma = 0;
    }

    return result;
}

Ina237Measurement DrvIna237::readMeasurement()
{
    Ina237Measurement meas = { 0, 0U, 0U, false };
    uint16_t raw_current = 0U;
    uint16_t raw_vbus    = 0U;
    uint16_t raw_power   = 0U;

    const common::Status s1 = m_i2c.readRegister(m_addr, REG_CURRENT, raw_current);
    const common::Status s2 = m_i2c.readRegister(m_addr, REG_VBUS, raw_vbus);
    const common::Status s3 = m_i2c.readRegister(m_addr, REG_POWER, raw_power);

    if ((s1 == common::Status::OK) &&
        (s2 == common::Status::OK) &&
        (s3 == common::Status::OK))
    {
        meas.current_ma     = ina237::convertCurrentRawToMa(
                                  static_cast<int16_t>(raw_current), m_currentLsbUa);
        meas.bus_voltage_mv = ina237::convertVbusRawToMv(raw_vbus);
        meas.power_mw       = ina237::convertPowerRawToMw(raw_power, m_currentLsbUa);
        meas.is_valid       = true;
    }

    return meas;
}

bool DrvIna237::selfTest()
{
    return m_i2c.isDevicePresent(m_addr);
}

} // namespace low
