// ============================================================================
// FILE: 60_valid/test/test_main.cpp
// PURPOSE: Tests unitaires des fonctions PURES (sans matériel ESP32)
// MODULES TESTÉS:
//   - low::ina237  (conversions registres → unités physiques)
//   - app::logic   (décision d'alarme courant + niveau combiné)
//
// COMPILATION (depuis 60_valid/test) :  make test
// Code retour : 0 si tous les tests passent, 1 sinon (CI).
// ============================================================================

#include <cstdio>
#include <cstdint>

#include "drv_ina237_convert.hpp"
#include "app_power_monitor_logic.hpp"
#include "AppConfig.hpp"

namespace {

uint32_t g_nb_ok = 0U;
uint32_t g_nb_ko = 0U;

void verifyU32(uint32_t expected, uint32_t actual, const char* name)
{
    if (expected == actual)
    {
        g_nb_ok++;
        (void)std::printf("[OK]   %s\n", name);
    }
    else
    {
        g_nb_ko++;
        (void)std::printf("[FAIL] %s (attendu=%u, obtenu=%u)\n",
                          name, static_cast<unsigned int>(expected),
                          static_cast<unsigned int>(actual));
    }
}

void verifyI32(int32_t expected, int32_t actual, const char* name)
{
    if (expected == actual)
    {
        g_nb_ok++;
        (void)std::printf("[OK]   %s\n", name);
    }
    else
    {
        g_nb_ko++;
        (void)std::printf("[FAIL] %s (attendu=%d, obtenu=%d)\n", name, expected, actual);
    }
}

void verifyBool(bool expected, bool actual, const char* name)
{
    if (expected == actual)
    {
        g_nb_ok++;
        (void)std::printf("[OK]   %s\n", name);
    }
    else
    {
        g_nb_ko++;
        (void)std::printf("[FAIL] %s (attendu=%d, obtenu=%d)\n",
                          name, static_cast<int>(expected), static_cast<int>(actual));
    }
}

void testConvertVbus()
{
    uint32_t r = 0U;
    r = low::ina237::convertVbusRawToMv(1000U);
    verifyU32(3125U, r, "convertVbusRawToMv(1000) == 3125 mV");
    r = low::ina237::convertVbusRawToMv(0U);
    verifyU32(0U, r, "convertVbusRawToMv(0) == 0 mV");
}

void testConvertCurrent()
{
    int32_t r = 0;
    r = low::ina237::convertCurrentRawToMa(500, 1000U);
    verifyI32(500, r, "convertCurrentRawToMa(500, 1000) == 500 mA");
    r = low::ina237::convertCurrentRawToMa(-200, 1000U);
    verifyI32(-200, r, "convertCurrentRawToMa(-200, 1000) == -200 mA");
}

void testConvertPower()
{
    uint32_t r = 0U;
    r = low::ina237::convertPowerRawToMw(100U, 1000U);
    verifyU32(2500U, r, "convertPowerRawToMw(100, 1000) == 2500 mW");
}

void testCheckCurrentAlarm()
{
    bool r = false;
    const int32_t seuil = 8000;
    r = app::logic::checkCurrentAlarm(5000, seuil);
    verifyBool(false, r, "checkCurrentAlarm(5000 < 8000) -> pas d'alarme");
    r = app::logic::checkCurrentAlarm(8000, seuil);
    verifyBool(true, r, "checkCurrentAlarm(8000 == 8000) -> alarme (limite incluse)");
    r = app::logic::checkCurrentAlarm(9000, seuil);
    verifyBool(true, r, "checkCurrentAlarm(9000 > 8000) -> alarme");
    r = app::logic::checkCurrentAlarm(-100, seuil);
    verifyBool(false, r, "checkCurrentAlarm(courant negatif) -> pas d'alarme");
}

void testEvaluateLevel()
{
    common::AlarmLevel lvl = common::AlarmLevel::NORMAL;

    lvl = app::logic::evaluateLevel(2000, 25.0F, 24.0F);
    verifyU32(static_cast<uint32_t>(common::AlarmLevel::NORMAL),
              static_cast<uint32_t>(lvl), "evaluateLevel(2A, 25C, 24C) -> NORMAL");

    lvl = app::logic::evaluateLevel(16000, 25.0F, 24.0F);
    verifyU32(static_cast<uint32_t>(common::AlarmLevel::WARNING),
              static_cast<uint32_t>(lvl), "evaluateLevel(16A, 25C, 24C) -> WARNING");

    lvl = app::logic::evaluateLevel(22000, 25.0F, 24.0F);
    verifyU32(static_cast<uint32_t>(common::AlarmLevel::CRITICAL),
              static_cast<uint32_t>(lvl), "evaluateLevel(22A, 25C, 24C) -> CRITICAL");

    lvl = app::logic::evaluateLevel(2000, 80.0F, 24.0F);
    verifyU32(static_cast<uint32_t>(common::AlarmLevel::CRITICAL),
              static_cast<uint32_t>(lvl), "evaluateLevel(2A, 80C, 24C) -> CRITICAL (temp)");
}

} // namespace

int main()
{
    (void)std::printf("=== Tests unitaires - modules purs C++ (sans materiel) ===\n\n");

    testConvertVbus();
    testConvertCurrent();
    testConvertPower();
    testCheckCurrentAlarm();
    testEvaluateLevel();

    (void)std::printf("\n=== Resultat : %u test(s) OK / %u test(s) en echec ===\n",
                      static_cast<unsigned int>(g_nb_ok),
                      static_cast<unsigned int>(g_nb_ko));

    return (g_nb_ko == 0U) ? 0 : 1;
}
