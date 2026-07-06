# projet_esp32_src — C++ (architecture app/mid/low/config)

Carte de monitoring AGV (Ventec Systems) en **C++**, combinant :

- l'**ossature de dossiers numérotés** du dépôt GitHub (`30_src/`, `60_valid/`…) ;
- l'**architecture en couches `app/mid/low/config`** validée par le prof ;
- la **machine à états** du diagramme `elec_FSM.pdf` au cœur de `MainApp`.

## Simplification : pas de wrappers I2C/ADC/UART

Les drivers ESP-IDF (`driver/i2c.h`, `driver/adc.h`, `driver/uart.h`) sont
**appelés directement** par les classes LOW qui en ont besoin (`drv_ina237`,
`drv_temp_sensor`, `drv_ihm`). Inutile de réécrire un wrapper par-dessus ce
que le framework fournit déjà.

## Structure

```
30_src/components/
│
├── app/                         COUCHE APP (logique métier + FSM)
│   ├── MainApp                  FSM (000→005 + erreurs 101→105)
│   ├── MonitoringService        Acquisition + alarme
│   ├── AlarmService             LED/buzzer/boutons
│   ├── EventLoggingService      (squelette)
│   ├── ConfigService            (squelette)
│   └── app_power_monitor_logic  (pur, testable)
│
├── mid/                         COUCHE MID (communication)
│   ├── i2c_manager              Table adresses I2C (EXF-31)
│   ├── ble_manager              (squelette)
│   └── usb_manager              (squelette)
│
├── low/                         COUCHE LOW (drivers hardware)
│   ├── hal/sensors/
│   │   ├── drv_ina237           Capteur courant (ESP-IDF I2C direct)
│   │   ├── drv_ina237_convert   Conversions pures (testable)
│   │   └── drv_temp_sensor      Sondes NTC (ESP-IDF ADC direct, squelette)
│   ├── hal/ihm/
│   │   └── drv_ihm              LED/buzzer/boutons (ESP-IDF GPIO direct)
│   └── memory/
│       ├── ring_buffer          Rotation FIFO (réel)
│       ├── psram_manager        (squelette)
│       └── flash_storage        (squelette)
│
└── config/                      COUCHE CONFIG (partagée)
    ├── common_types             Status, SystemState, enums
    ├── HardwareConfig           Broches GPIO, adresses I2C
    ├── AppConfig                Seuils métier
    └── SystemConfig             Config générale
```

## Règle d'or

```
APP  →  MID  →  LOW  →  ESP-IDF (hardware)
        ↘  CONFIG (partagé, inclus par toutes)
```

## Tester (PC, sans ESP32)

```bash
cd 60_valid/test
make test
```

Résultat : `13 test(s) OK / 0 test(s) en echec`.

## Compiler pour ESP32

```bash
cd 30_src
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSBx flash monitor
```
