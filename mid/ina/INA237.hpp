#pragma once

#include <cstdint>

#include "I2CBus.hpp"
#include "INA237Registers.hpp"

/**
 * @brief Driver du moniteur de puissance INA237.
 */
class INA237
{
public:
    /**
     * @brief Constructeur.
     *
     * @param bus Référence vers le bus I²C.
     * @param address Adresse I²C du composant.
     */
    explicit INA237(I2CBus &bus,
                    uint8_t address = INA237Defs::ADDRESS);

    static constexpr uint8_t ADDRESS = INA237Defs::ADDRESS;

    bool begin();

    bool isConnected();

    bool reset();

    void setCalibration(uint32_t shunt_uohm,
                        uint32_t max_current_ma);

    float readBusVoltage();

    float readShuntVoltage();

    float readCurrent();

    float readPower();

    float readTemperature();

private:
    I2CBus &_bus;

    uint8_t _address;

    float _current_lsb = 0.001f;

    float _power_lsb = 0.001f;
};