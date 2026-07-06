# Règles MISRA appliquées (version C++)

Ce projet applique manuellement les règles MISRA les plus importantes, adaptées
au C++. Chaque règle est illustrée par un exemple tiré du code livré. Pour une
conformité complète, un analyseur statique dédié (Cppcheck addon MISRA, PC-lint,
Polyspace) reste nécessaire en CI.

## 1. Types explicites
On utilise `cstdint` (`uint8_t`, `int32_t`…) et l'alias `float32_t` plutôt que
`int`/`float` nus, dont la taille dépend de la cible.

```cpp
using float32_t = float;
int32_t  current_ma = 0;
uint32_t bus_voltage_mv = 0U;
```

## 2. Pas de magic numbers
Toute valeur significative (adresse, broche, seuil) est une constante nommée
dans `common_config.hpp`.

```cpp
constexpr uint8_t INA237_I2C_ADDR = 0x40U;
constexpr int32_t CURRENT_CRITICAL_MA = 20000;
```

## 3. Accolades obligatoires
Tout `if`/`else`/`switch` utilise des accolades, même pour une seule instruction.

```cpp
if (current_ma >= threshold_ma)
{
    alarm = true;
}
else
{
    alarm = false;
}
```

## 4. Point de sortie unique
Chaque fonction déclare sa variable de retour au début et la renseigne, avec un
seul `return` final.

```cpp
common::Status init()
{
    common::Status result = common::Status::OK;
    if (/* échec */) { result = common::Status::ERROR_INIT; }
    return result;
}
```

## 5. enum class plutôt que #define
Les énumérations sont typées et scellées dans un namespace : pas de conversion
implicite, pas de collision de noms.

```cpp
enum class SystemState : uint8_t { POWER_SELECT = 0U, /* … */ };
```

## 6. Initialisation systématique
Toute variable locale est initialisée à sa déclaration.

```cpp
uint16_t raw = 0U;
MonitorData data = { 0, 0U, 0.0F, 0.0F, common::AlarmLevel::NORMAL, false };
```

## 7. Include guards sur tous les headers
Chaque `.hpp` est protégé contre les inclusions multiples.

```cpp
#ifndef APP_MAIN_HPP
#define APP_MAIN_HPP
/* … */
#endif // APP_MAIN_HPP
```

## 8. Séparation stricte des couches
`app` n'inclut jamais les headers ESP-IDF (`driver/i2c.h`, `driver/gpio.h`) : seul
`midlevel` le fait. Aucune dépendance remontante (`midlevel` n'inclut jamais
`app`).

## 9. Pas d'allocation dynamique
Aucun `new`/`delete`/`malloc`. L'objet applicatif est statique
(`static app::AppMain g_app;`), les drivers sont injectés par référence.

## 10. Branche default défensive
Chaque `switch` sur un `enum` possède un `default` qui sécurise l'état en cas de
corruption mémoire.

```cpp
default:
    logError(m_state);
    m_state = common::SystemState::ERR_RUNTIME;
    break;
```

## 11. Pas de while(1) nu
La boucle principale a une condition d'arrêt explicite (`m_running`), pas de
`while(1)` sans sortie.

## 12. Pointeurs / références vérifiés
Les dépendances sont passées par référence (jamais de pointeur propriétaire nu),
ce qui élimine le risque de déréférencement nul côté drivers.

## Limites
Ces règles sont appliquées au niveau d'un projet pédagogique. Une certification
MISRA-C++ industrielle exige un outil d'analyse statique intégré à la CI.
