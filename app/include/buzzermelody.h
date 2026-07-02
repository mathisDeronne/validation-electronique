#ifndef BUZZER_H
#define BUZZER_H

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_rom_sys.h>

#include "HardwareConfig.h"

//==============================================================
// NOTES
//==============================================================
#define NOTE_DO4 262
#define NOTE_RE4 294
#define NOTE_MI4 330
#define NOTE_FA4 349
#define NOTE_SOL4 392
#define NOTE_LA4 440
#define NOTE_SI4 494
#define NOTE_DO5 523
#define NOTE_LA5 880

//==============================================================
// TYPE D'ALARME
//==============================================================
enum class AlarmSound
{
    TripleBeep,
    Siren,
    Warning,
    Emergency
};

//==============================================================
// JOUER UNE NOTE
//==============================================================
inline void playTone(gpio_num_t pin, int frequency, int duration_ms)
{
    if (frequency == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
        return;
    }

    int period = 1000000 / frequency;
    int halfPeriod = period / 2;
    int cycles = frequency * duration_ms / 1000;

    for (int i = 0; i < cycles; i++)
    {
        gpio_set_level(pin, 1);
        esp_rom_delay_us(halfPeriod);

        gpio_set_level(pin, 0);
        esp_rom_delay_us(halfPeriod);
    }
}

//==============================================================
// MELODIE 1 : Triple Bip -> TripleBeep
//==============================================================
inline void melodyTripleBeep()
{
    for (int i = 0; i < 3; i++)
    {
        playTone(HardwareConfig::IHM::BUZZER, NOTE_DO5, 150);
        vTaskDelay(pdMS_TO_TICKS(80));
    }
}

//==============================================================
// MELODIE 2 : Sirène -> Siren
//==============================================================
inline void melodySiren()
{
    for (int i = 0; i < 6; i++)
    {
        playTone(HardwareConfig::IHM::BUZZER, NOTE_LA4, 200);
        playTone(HardwareConfig::IHM::BUZZER, NOTE_LA5, 200);
    }
}

//==============================================================
// MELODIE 3 : Alerte montante -> Warning
//==============================================================
inline void melodyWarning()
{
    playTone(HardwareConfig::IHM::BUZZER, NOTE_DO4, 150);
    playTone(HardwareConfig::IHM::BUZZER, NOTE_MI4, 150);
    playTone(HardwareConfig::IHM::BUZZER, NOTE_SOL4, 150);
    playTone(HardwareConfig::IHM::BUZZER, NOTE_DO5, 500);
}

//==============================================================
// MELODIE 4 : Sirène montante / descendante - > Emergency
//==============================================================
inline void melodyEmergency()
{
    for (int f = 500; f <= 1500; f += 50)
    {
        playTone(HardwareConfig::IHM::BUZZER, f, 20);
    }

    for (int f = 1500; f >= 500; f -= 50)
    {
        playTone(HardwareConfig::IHM::BUZZER, f, 20);
    }
}

//==============================================================
// CHOIX DE LA MELODIE
//==============================================================
inline void playAlarm(AlarmSound sound)
{
    switch (sound)
    {
    case AlarmSound::TripleBeep:
        melodyTripleBeep();
        break;

    case AlarmSound::Siren:
        melodySiren();
        break;

    case AlarmSound::Warning:
        melodyWarning();
        break;

    case AlarmSound::Emergency:
        melodyEmergency();
        break;
    }
}

#endif