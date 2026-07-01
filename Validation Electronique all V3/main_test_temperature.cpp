// ============================================================================
// FILE: main/main_test_temperature.cpp
// PURPOSE: Test des 3 capteurs de température sur le port série
//
// UTILISATION :
//   1. Renomme ton main.cpp actuel en main_backup.cpp
//   2. Renomme ce fichier en main.cpp
//   3. Flash : idf.py build && idf.py flash monitor
//   4. Ouvre le moniteur série (115200 bauds)
//   5. Tu verras les mesures défiler toutes les secondes
//   6. Quand tu as fini, remets ton main.cpp d'origine
//
// SORTIE ATTENDUE :
//   ╔══════════════════════════════════════════════════╗
//   ║  TEST CAPTEURS TEMPERATURE - Ventec Systems     ║
//   ╚══════════════════════════════════════════════════╝
//
//   [  1] NTC1(B600)= 24.8°C | NTC2(B601)= 25.1°C | TMP126(B501)= 23.5°C
//   [  2] NTC1(B600)= 24.9°C | NTC2(B601)= 25.0°C | TMP126(B501)= 23.5°C
//   ...
// ============================================================================

#include <cstdio>
#include <cstdint>

#include "common_types.hpp"
#include "HardwareConfig.hpp"
#include "AppConfig.hpp"
#include "drv_temp_sensor.hpp"
#include "drv_tmp126.hpp"

// Sur cible ESP32 réelle, décommenter :
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/i2c.h"
// #include "esp_log.h"

// Tag pour les logs ESP-IDF
// static const char* TAG = "TEMP_TEST";

namespace {

// ============================================================================
// Initialisation du bus I2C (nécessaire avant de lire l'INA237 / autres I2C)
// ============================================================================
void initI2cBus()
{
    // --- Cible réelle ESP-IDF ---
    // i2c_config_t conf = {};
    // conf.mode = I2C_MODE_MASTER;
    // conf.sda_io_num = GPIO_NUM_21;
    // conf.scl_io_num = GPIO_NUM_22;
    // conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    // conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    // conf.master.clk_speed = 400000;  // 400 kHz
    // i2c_param_config(I2C_NUM_0, &conf);
    // i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    std::printf("[INIT] Bus I2C initialise\n");
}

// ============================================================================
// Affichage d'un en-tête clair sur le port série
// ============================================================================
void printHeader()
{
    std::printf("\n");
    std::printf("======================================================\n");
    std::printf("  TEST CAPTEURS TEMPERATURE - Ventec Systems\n");
    std::printf("======================================================\n");
    std::printf("\n");
    std::printf("  NTC1 (B600) : NB12K00103JBB sur ADC (GPIO %u)\n",
                config::hardware::ADC_NTC_1);
    std::printf("  NTC2 (B601) : NB12K00103JBB sur ADC (GPIO %u)\n",
                config::hardware::ADC_NTC_2);
    std::printf("  TMP126 (B501) : TMP126DCKR sur SPI (CS=GPIO%u SCK=GPIO%u)\n",
                config::hardware::SPI_TMP126_CS,
                config::hardware::SPI_TMP126_SCK);
    std::printf("\n");
    std::printf("  Seuil WARNING  : %.1f C\n", config::app::TEMP_WARNING_C);
    std::printf("  Seuil CRITICAL : %.1f C\n", config::app::TEMP_CRITICAL_C);
    std::printf("\n");
    std::printf("------------------------------------------------------\n");
    std::printf("  #   | NTC1(B600) | NTC2(B601) | TMP126    | ALARME\n");
    std::printf("------------------------------------------------------\n");
}

// ============================================================================
// Déterminer le texte d'alarme
// ============================================================================
const char* alarmText(common::float32_t ntc1,
                      common::float32_t ntc2,
                      common::float32_t tmp126)
{
    const char* result = "OK";

    // On prend la plus haute des 2 NTC (température PCB)
    common::float32_t max_pcb = ntc1;
    if (ntc2 > ntc1)
    {
        max_pcb = ntc2;
    }

    if ((max_pcb >= config::app::TEMP_CRITICAL_C) ||
        (tmp126 >= config::app::TEMP_CRITICAL_C))
    {
        result = "!! CRITIQUE !!";
    }
    else if ((max_pcb >= config::app::TEMP_WARNING_C) ||
             (tmp126 >= config::app::TEMP_WARNING_C))
    {
        result = "! WARNING !";
    }
    else
    {
        result = "OK";
    }

    return result;
}

} // namespace

// ============================================================================
// POINT D'ENTRÉE
// ============================================================================
extern "C" void app_main(void)
{
    // Attendre que le port série soit prêt
    // vTaskDelay(pdMS_TO_TICKS(1000));

    // Initialisation du bus I2C
    initI2cBus();

    // Création des 3 capteurs
    low::DrvNtcSensor ntc1(config::hardware::ADC_NTC_1);   // B600
    low::DrvNtcSensor ntc2(config::hardware::ADC_NTC_2);   // B601
    low::DrvTmp126    tmp126;                                // B501

    // Initialisation
    std::printf("[INIT] Initialisation NTC1 (B600)... ");
    if (ntc1.init() == common::Status::OK)
    {
        std::printf("OK\n");
    }
    else
    {
        std::printf("ERREUR\n");
    }

    std::printf("[INIT] Initialisation NTC2 (B601)... ");
    if (ntc2.init() == common::Status::OK)
    {
        std::printf("OK\n");
    }
    else
    {
        std::printf("ERREUR\n");
    }

    std::printf("[INIT] Initialisation TMP126 (B501)... ");
    if (tmp126.init() == common::Status::OK)
    {
        std::printf("OK\n");
    }
    else
    {
        std::printf("ERREUR\n");
    }

    std::printf("[INIT] Self-test TMP126... ");
    if (tmp126.selfTest())
    {
        std::printf("OK\n");
    }
    else
    {
        std::printf("ECHEC (capteur absent ?)\n");
    }

    // En-tête du tableau
    printHeader();

    // Boucle de lecture toutes les secondes
    uint32_t count = 0U;
    bool running = true;

    while (running)
    {
        count++;

        // Lecture des 3 capteurs
        const common::float32_t t_ntc1   = ntc1.readTemperatureC();
        const common::float32_t t_ntc2   = ntc2.readTemperatureC();
        const common::float32_t t_tmp126 = tmp126.readTemperatureC();

        // Déterminer l'alarme
        const char* alarm = alarmText(t_ntc1, t_ntc2, t_tmp126);

        // Affichage sur le port série
        std::printf("[%3u] NTC1(B600)=%6.1f C | NTC2(B601)=%6.1f C | TMP126=%6.1f C | %s\n",
                    static_cast<unsigned int>(count),
                    t_ntc1,
                    t_ntc2,
                    t_tmp126,
                    alarm);

        // Séparateur toutes les 10 lignes (lisibilité)
        if ((count % 10U) == 0U)
        {
            std::printf("------------------------------------------------------\n");
        }

        // Attente 1 seconde
        // --- Cible réelle ESP-IDF ---
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // --- Mock PC (pour test compilation) ---
        if (count >= 5U)
        {
            running = false; // Sur PC, on s'arrête après 5 mesures
        }
    }

    std::printf("\n[FIN] Test termine. %u mesures effectuees.\n",
                static_cast<unsigned int>(count));
}
