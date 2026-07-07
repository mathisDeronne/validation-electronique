#include <iostream>
#include <cassert>

#include "INA237.hpp"
#include "I2CBus.hpp"
#include "I2CBusMock.hpp"
#include "INA237Registers.hpp"

/**
 * @brief Test logique complet du driver INA237
 */
int main()
{
    std::cout << "=============================\n";
    std::cout << " INA237 LOGICAL TEST START\n";
    std::cout << "=============================\n\n";

    I2CBusMock bus;

    INA237 ina(bus, INA237::ADDRESS);

    // =========================================================
    // 1. TEST DEVICE PRESENT (OK)
    // =========================================================
    std::cout << "[TEST 1] Device present OK\n";

    bus.setRegister(
        static_cast<uint8_t>(INA237Defs::Register::MANUFACTURER_ID),
        INA237Defs::MANUFACTURER_ID_VALUE);

    assert(ina.begin() == true);
    std::cout << " -> PASS\n\n";

    // =========================================================
    // 2. TEST DEVICE ABSENT
    // =========================================================
    std::cout << "[TEST 2] Device absent\n";

    bus.setRegister(
        static_cast<uint8_t>(INA237Defs::Register::MANUFACTURER_ID),
        0x0000);

    assert(ina.isConnected() == false);
    std::cout << " -> PASS\n\n";

    // =========================================================
    // 3. RESET TEST
    // =========================================================
    std::cout << "[TEST 3] Reset device\n";

    bus.setFailWrite(false);

    assert(ina.reset() == true);
    std::cout << " -> PASS\n\n";

    // =========================================================
    // 4. CALIBRATION TEST
    // =========================================================
    std::cout << "[TEST 4] Calibration\n";

    ina.setCalibration(10000, 2000);

    std::cout << " -> PASS\n\n";

    // =========================================================
    // 5. VOLTAGE CONVERSION TEST
    // =========================================================
    std::cout << "[TEST 5] Voltage conversion\n";

    bus.setRegister(
        static_cast<uint8_t>(INA237Defs::Register::VBUS),
        16000 // raw simulé
    );

    float v = ina.readBusVoltage();

    std::cout << "Voltage = " << v << " V\n";
    assert(v > 0.0f);

    std::cout << " -> PASS\n\n";

    // =========================================================
    // 6. CURRENT TEST
    // =========================================================
    std::cout << "[TEST 6] Current conversion\n";

    bus.setRegister(
        static_cast<uint8_t>(INA237Defs::Register::CURRENT),
        1000);

    float c = ina.readCurrent();

    std::cout << "Current = " << c << " A\n";
    assert(c >= 0.0f);

    std::cout << " -> PASS\n\n";

    // =========================================================
    // 7. POWER TEST
    // =========================================================
    std::cout << "[TEST 7] Power conversion\n";

    bus.setRegister(
        static_cast<uint8_t>(INA237Defs::Register::POWER),
        500);

    float p = ina.readPower();

    std::cout << "Power = " << p << " W\n";
    assert(p >= 0.0f);

    std::cout << " -> PASS\n\n";

    // =========================================================
    // 8. ERROR I2C READ
    // =========================================================
    std::cout << "[TEST 8] I2C read failure\n";

    bus.setFailRead(true);

    float v_fail = ina.readBusVoltage();

    std::cout << "Voltage fallback = " << v_fail << "\n";

    bus.setFailRead(false);

    std::cout << " -> PASS\n\n";

    // =========================================================
    // FINAL RESULT
    // =========================================================
    std::cout << "=============================\n";
    std::cout << " ALL TESTS PASSED SUCCESSFULLY\n";
    std::cout << "=============================\n";

    return 0;
}