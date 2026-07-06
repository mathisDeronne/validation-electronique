# Architecture logicielle — projet_esp32_src (C++, version simplifiée)

## 1. Choix : pas de wrappers I2C/ADC/UART

ESP-IDF fournit déjà des drivers I2C, ADC et UART complets. Les encapsuler
dans des classes C++ n'apportait rien : c'était du code supplémentaire sans
valeur ajoutée. Les drivers LOW (`drv_ina237`, `drv_temp_sensor`, `drv_ihm`)
appellent directement les fonctions ESP-IDF.

Résultat : 3 dossiers en moins, 6 fichiers en moins, code plus simple.

## 2. Les 4 couches

| Couche | Contenu | Dépend de |
|--------|---------|-----------|
| **config** | Types, constantes, seuils | rien |
| **low** | Drivers capteurs, IHM, mémoire | config + ESP-IDF |
| **mid** | Table I2C, BLE, USB | config |
| **app** | FSM, services, logique pure | config + low + mid |

## 3. FSM principale (MainApp)

Reprise fidèle du diagramme `elec_FSM.pdf` :

```
000 power select → 001 test programme → 002 init hardware
   → 003 init comm → 004 start appmain → 005 run
```

Chaque test qui échoue bascule vers l'état d'erreur correspondant (101→105).
Le bouton reset (« bp clear ») ramène toujours à l'état 001 (test programme).

## 4. Code réel vs squelettes

| Module | Statut | Testable hors cible ? |
|--------|--------|-----------------------|
| MainApp (FSM) | réel | oui (FSM tourne sur PC) |
| app_power_monitor_logic | réel | oui (fonctions pures) |
| drv_ina237_convert | réel | oui (fonctions pures) |
| MonitoringService | réel | non (dépend des drivers) |
| AlarmService | réel | non |
| drv_ina237 | réel (mock I2C) | non |
| drv_ihm | réel (mock GPIO) | non |
| i2c_manager | réel | oui (logique pure) |
| ring_buffer | réel | oui (template pur) |
| BLE / USB / PSRAM / Flash / NTC | squelettes | non |

Les squelettes compilent et s'intègrent dans la FSM. Chaque endroit à
compléter est marqué par un commentaire `--- Cible réelle ESP-IDF ---`.

## 5. Validation

- 13 tests unitaires passent (conversions INA237 + logique d'alarme).
- La FSM exécutée sur PC enchaîne 000 → 001 → 002 → 003 → 004 → 005 → RUN.
- Toute l'arborescence compile sans warning (`-Wall -Wextra -Wpedantic`).
- 0 fichier C : 17 `.cpp` + 20 `.hpp`.
