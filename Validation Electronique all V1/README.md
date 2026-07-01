# projet_esp32_src — C++ (architecture app/mid/low/config)

Carte de monitoring AGV (Ventec Systems) en **C++**, combinant :

- l'**ossature de dossiers numérotés** du dépôt GitHub (`30_src/`, `60_valid/`…) ;
- l'**architecture en couches `app/mid/low/config`** validée par le prof ;
- la **machine à états** du diagramme `elec_FSM.pdf` au cœur de `MainApp`.

## Structure

```
30_src/
├── main/
│   ├── main.cpp                       Point d'entrée ESP-IDF (extern "C" app_main)
│   └── CMakeLists.txt
├── components/
│   ├── app/                           COUCHE APP (logique métier + FSM)
│   │   ├── include/
│   │   │   ├── MainApp.hpp            FSM principale (000→005 + 101→105)
│   │   │   ├── MonitoringService.hpp
│   │   │   ├── AlarmService.hpp
│   │   │   ├── EventLoggingService.hpp        (squelette)
│   │   │   ├── ConfigService.hpp              (squelette)
│   │   │   └── app_power_monitor_logic.hpp    (pur, testable)
│   │   ├── MainApp.cpp
│   │   ├── MonitoringService.cpp
│   │   ├── AlarmService.cpp
│   │   ├── EventLoggingService.cpp
│   │   ├── ConfigService.cpp
│   │   ├── app_power_monitor_logic.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── mid/                           COUCHE MID (communication)
│   │   ├── include/
│   │   │   ├── i2c_manager.hpp        (réel : bus + table adresses EXF-31)
│   │   │   ├── ble_manager.hpp        (squelette)
│   │   │   └── usb_manager.hpp        (squelette)
│   │   ├── i2c_manager.cpp
│   │   ├── ble_manager.cpp
│   │   ├── usb_manager.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── low/                           COUCHE LOW (drivers hardware)
│   │   ├── hal/
│   │   │   ├── i2c/      drv_i2c          (réel)
│   │   │   ├── sensors/  drv_ina237       (réel) + drv_ina237_convert (pur)
│   │   │   │             drv_temp_sensor  (squelette NTC)
│   │   │   ├── ihm/      drv_ihm          (réel : LED/buzzer/boutons)
│   │   │   ├── adc/      drv_adc          (squelette)
│   │   │   └── uart/     drv_uart         (squelette)
│   │   ├── memory/
│   │   │   ├── ring_buffer.hpp        (réel : rotation FIFO EXF-18)
│   │   │   ├── psram_manager          (squelette EXF-15..17)
│   │   │   └── flash_storage          (squelette EXF-23)
│   │   └── CMakeLists.txt
│   │
│   └── config/                        COUCHE CONFIG (partagée)
│       ├── include/
│       │   ├── common_types.hpp       Status, SystemState, enums
│       │   ├── HardwareConfig.hpp     Broches GPIO, adresses I2C
│       │   ├── AppConfig.hpp          Seuils métier
│       │   └── SystemConfig.hpp       Config générale (périodes, LSB)
│       └── CMakeLists.txt
└── CMakeLists.txt

60_valid/test/                         Tests unitaires C++ (fonctions pures)
20_doc_int/                            Documentation interne
```

## Les 4 couches (règle d'or)

```
APP  →  MID  →  LOW  →  Hardware
        ↘  CONFIG (partagé, inclus par toutes)
```

`app` ne touche jamais le matériel directement, il passe toujours par `mid` ou
`low`. Aucune dépendance remontante (`low` n'inclut jamais `mid` ni `app`).

## Code réel vs squelettes

Conformément au plan retenu :

- **code réel** : FSM (`MainApp`), conversions INA237, logique d'alarme, driver
  I2C, driver INA237, driver IHM, gestionnaire I2C (table d'adresses), ring
  buffer ;
- **squelettes commentés** (interface complète + `TODO` cible) : BLE, USB, ADC,
  UART, capteur NTC, PSRAM, Flash, services Event/Config.

Chaque squelette compile et s'intègre dans la FSM ; il ne reste qu'à remplir les
blocs `TODO` / `--- Cible réelle ESP-IDF ---` sur la vraie carte.

## Machine à états (MainApp)

```
000 power select → 001 test programme → 002 init hardware
   → 003 init comm → 004 start appmain → 005 run
```

Chaque test qui échoue bascule vers l'état d'erreur 101→105 (log + attente du
bouton reset « bp clear » pour revenir à 001).

## Compiler les tests (PC, sans ESP32)

```bash
cd 60_valid/test
make test
```

Résultat attendu : `13 test(s) OK / 0 test(s) en echec`.

## Compiler pour ESP32

```bash
cd 30_src
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSBx flash monitor
```
