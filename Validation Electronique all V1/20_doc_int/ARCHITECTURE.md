# Architecture du projet — correspondance et choix

Ce document explique la structure retenue : l'ossature de dossiers vient du dépôt
GitHub, l'organisation des couches vient de l'architecture validée par le prof.

## 1. Deux sources combinées

| Élément                 | Origine                          |
|-------------------------|----------------------------------|
| Dossiers `30_src/`, `60_valid/`, `20_doc_int/` | Dépôt GitHub `projet_esp32_src` |
| Couches `app / mid / low / config`             | Architecture vérifiée par le prof |
| FSM `MainApp` (000→005 + 101→105)              | Diagramme `elec_FSM.pdf`         |
| Style classes C++, `enum class`, `float32_t`   | EnergyManager                    |

## 2. Les 4 couches

- **app** — logique métier : FSM (`MainApp`), services (`MonitoringService`,
  `AlarmService`, `EventLoggingService`, `ConfigService`) et logique pure
  (`app_power_monitor_logic`).
- **mid** — communication : `I2cManager` (bus + table d'adresses EXF-31),
  `BleManager`, `UsbManager`.
- **low** — drivers hardware : `drv_i2c`, `drv_ina237` (+ `drv_ina237_convert`),
  `drv_temp_sensor`, `drv_ihm`, `drv_adc`, `drv_uart`, et `memory`
  (`ring_buffer`, `psram_manager`, `flash_storage`).
- **config** — partagée : `common_types`, `HardwareConfig`, `AppConfig`,
  `SystemConfig`.

Règle d'or : `app → mid → low → hardware`, `config` partagé par tous. Aucune
dépendance remontante.

## 3. Code réel vs squelettes

| Module                     | Couche | Statut     |
|----------------------------|--------|------------|
| MainApp (FSM)              | app    | réel       |
| app_power_monitor_logic    | app    | réel (pur) |
| MonitoringService          | app    | réel       |
| AlarmService               | app    | réel       |
| EventLoggingService        | app    | squelette  |
| ConfigService              | app    | squelette  |
| I2cManager                 | mid    | réel       |
| BleManager                 | mid    | squelette  |
| UsbManager                 | mid    | squelette  |
| drv_i2c                    | low    | réel       |
| drv_ina237 / convert       | low    | réel       |
| drv_ihm                    | low    | réel       |
| ring_buffer                | low    | réel       |
| drv_temp_sensor            | low    | squelette  |
| drv_adc / drv_uart         | low    | squelette  |
| psram_manager / flash      | low    | squelette  |

Les squelettes exposent une interface complète et un corps avec `TODO` /
`--- Cible réelle ESP-IDF ---`. Ils compilent et s'intègrent dans la FSM.

## 4. Correspondance avec les noms du dépôt git

Les noms de fichiers gardent le style du dépôt (`drv_*`), seules les **couches**
changent : `midlevel` du dépôt devient `mid` + `low` (séparation pilotes de bus
/ pilotes de périphérique), et `common` devient `config`.

| Dépôt git (C)            | Ici (C++)                              |
|--------------------------|----------------------------------------|
| `common/`                | `config/`                              |
| `midlevel/drv_i2c`       | `low/hal/i2c/drv_i2c`                  |
| `midlevel/drv_ina237`    | `low/hal/sensors/drv_ina237`           |
| `midlevel/drv_ihm`       | `low/hal/ihm/drv_ihm`                  |
| `app/app_main`           | `app/MainApp`                          |
| `app/app_power_monitor`  | `app/MonitoringService`                |
| `app/app_ihm`            | `app/AlarmService`                     |

## 5. Validation

- Tests unitaires : `13 OK / 0 échec` (conversions + logique d'alarme).
- FSM exécutée sur PC : enchaîne `000 → 001 → 002 → 003 → 004 → 005` puis reste
  en RUN, conforme au diagramme.
- Toute l'arborescence (4 couches + main) compile sans warning avec
  `-Wall -Wextra -Wpedantic`.
