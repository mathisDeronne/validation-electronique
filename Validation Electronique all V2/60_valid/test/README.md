# Tests unitaires — projet_esp32_src (C++)

Tests des fonctions purement logicielles (sans dépendance matérielle ESP32).

## Modules testés (fonctions pures)

- `30_src/components/low/hal/sensors/drv_ina237_convert.cpp` — conversions
  registres bruts INA237 → mV / mA / mW.
- `30_src/components/app/app_power_monitor_logic.cpp` — décision d'alarme courant
  et calcul du niveau combiné courant + température.

Ces fonctions n'ont aucune E/S : même entrée → même sortie. Elles se compilent
et s'exécutent directement sur PC avec g++.

## Compiler et lancer

```bash
cd 60_valid/test
make test
```

Sortie attendue : `13 test(s) OK / 0 test(s) en echec`.

Le code de retour est `0` si tout passe, `1` sinon (utilisable en CI).

## Nettoyer

```bash
make clean
```
