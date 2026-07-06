// ============================================================================
// FILE: components/config/include/common_types.hpp
// LAYER: CONFIG (partagé par toutes les couches app/mid/low)
// PURPOSE: Types d'erreur, énumérations et alias partagés
// MISRA: Règle types explicites + include guard
// ============================================================================

#ifndef COMMON_TYPES_HPP
#define COMMON_TYPES_HPP

#include <cstdint>

namespace common {

// Type flottant explicite (MISRA : pas de 'float' nu)
using float32_t = float;

// Code d'erreur unifié pour toutes les couches
enum class Status : uint8_t {
    OK = 0U,
    ERROR,
    ERROR_NULL_POINTER,
    ERROR_I2C,
    ERROR_TIMEOUT,
    ERROR_INIT
};

// États de la FSM principale (diagramme elec_FSM.pdf)
//  000 POWER_SELECT / 001 TEST_PROGRAM / 002 INIT_HARDWARE
//  003 INIT_COMM / 004 START_APPMAIN / 005 RUN
//  101..105 : états d'erreur correspondants
enum class SystemState : uint8_t {
    POWER_SELECT   = 0U,
    TEST_PROGRAM   = 1U,
    INIT_HARDWARE  = 2U,
    INIT_COMM      = 3U,
    START_APPMAIN  = 4U,
    RUN            = 5U,

    ERR_PROGRAM    = 101U,
    ERR_INIT       = 102U,
    ERR_COMM       = 103U,
    ERR_APPMAIN    = 104U,
    ERR_RUNTIME    = 105U
};

// Modes IHM (LED + buzzer)
enum class HmiMode : uint8_t {
    OFF = 0U,
    SUCCESS_BEEP,
    LED_GREEN_ON,
    LED_ORANGE_BLINK_BUZZER_SLOW,
    LED_RED_ON_BUZZER_FAST
};

// Niveau d'alarme métier
enum class AlarmLevel : uint8_t {
    NORMAL = 0U,
    WARNING,
    CRITICAL
};

} // namespace common

#endif // COMMON_TYPES_HPP
