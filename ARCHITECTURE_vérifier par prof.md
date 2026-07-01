════════════════════════════════════════════════════════════════════════════

&#x20; VENTEC ARCHITECTURE - VERSION SIMPLIFIÉE

════════════════════════════════════════════════════════════════════════════



🎯 EN 30 SECONDES:



&#x20;   ┌─────────────────────────┐

&#x20;   │ APP (Logique métier)    │  ← Ce que tu veux faire

&#x20;   └──────────────┬──────────┘

&#x20;                  │ utilise

&#x20;   ┌──────────────▼──────────┐

&#x20;   │ MID (Communication)     │  ← Comment communiquer

&#x20;   └──────────────┬──────────┘

&#x20;                  │ utilise

&#x20;   ┌──────────────▼──────────┐

&#x20;   │ LOW (Drivers)           │  ← Accès au hardware

&#x20;   └──────────────┬──────────┘

&#x20;                  │

&#x20;   ┌──────────────▼──────────┐

&#x20;   │ Hardware (Puces ESP32)  │  ← Matériel physique

&#x20;   └─────────────────────────┘





════════════════════════════════════════════════════════════════════════════

&#x20; LES 3 COUCHES EXPLIQUÉES

════════════════════════════════════════════════════════════════════════════



1️⃣ LOW LAYER (Drivers)

&#x20;  ├─ Fait: Accède au hardware

&#x20;  ├─ Exemple: 

&#x20;  │   • Lire le capteur courant (INA237)

&#x20;  │   • Allumer la LED rouge

&#x20;  │   • Envoyer un message I2C

&#x20;  └─ Fichiers:

&#x20;      low/hal/sensors/CurrentSensorDriver.h

&#x20;      low/hal/gpio/LED.h

&#x20;      low/hal/uart/UARTDriver.h



2️⃣ MID LAYER (Protocoles)

&#x20;  ├─ Fait: Gérer la communication

&#x20;  ├─ Exemple:

&#x20;  │   • Envoyer données via BLE

&#x20;  │   • Recevoir commandes USB

&#x20;  │   • Gérer le bus I2C

&#x20;  └─ Fichiers:

&#x20;      mid/ble/BLEManager.h

&#x20;      mid/usb/USBManager.h

&#x20;      mid/i2c/I2CManager.h



3️⃣ APP LAYER (Métier)

&#x20;  ├─ Fait: La logique de l'application

&#x20;  ├─ Exemple:

&#x20;  │   • Mesurer le courant toutes les 100ms

&#x20;  │   • Si courant > 20A → ALARME!

&#x20;  │   • Enregistrer l'alerte dans l'historique

&#x20;  └─ Fichiers:

&#x20;      app/services/MonitoringService.h

&#x20;      app/services/AlarmService.h

&#x20;      app/src/MainApp.cpp





════════════════════════════════════════════════════════════════════════════

&#x20; EXEMPLE CONCRET: Mesure → Alerte

════════════════════════════════════════════════════════════════════════════



Scénario: Le courant est à 25A (seuil = 20A) → Alarme!



ÉTAPE 1: APP demande une mesure

&#x20; MainApp::run()

&#x20;   └─→ "MonitoringService, lis le courant!"



ÉTAPE 2: APP utilise LOW pour mesurer

&#x20; MonitoringService::update()

&#x20;   └─→ "CurrentSensorDriver, donne-moi le courant!"

&#x20;       └─→ \[LOW] Lit l'INA237 via I2C

&#x20;       └─→ Retourne 25.0A



ÉTAPE 3: APP vérifie le seuil

&#x20; AlarmService::checkAlarms(25.0A)

&#x20;   └─→ 25.0A > 20.0A ? OUI!

&#x20;   └─→ "LED, deviens ROUGE!"

&#x20;   └─→ "Buzzer, joue un bip!"

&#x20;   └─→ "BLEManager, notifie le client!"



ÉTAPE 4: MID envoie l'alerte

&#x20; BLEManager::notifyAlarm()

&#x20;   └─→ \[MID] Envoie notification Bluetooth

&#x20;   └─→ Tablette reçoit l'alerte ✓



ÉTAPE 5: APP enregistre l'événement

&#x20; EventLoggingService::logEvent()

&#x20;   └─→ \[LOW] Écrit en PSRAM avec timestamp ✓



RÉSULTAT:

✓ LED rouge allumée

✓ Buzzer actif

✓ Client BLE notifié

✓ Événement enregistré





════════════════════════════════════════════════════════════════════════════

&#x20; STRUCTURE DES DOSSIERS (SIMPLE)

════════════════════════════════════════════════════════════════════════════



VentecMonitoring/

│

├── app/                    ← LOGIQUE MÉTIER

│   ├── services/

│   │   ├── MonitoringService.h      (Mesure courant/temp)

│   │   ├── AlarmService.h           (Détecte alarmes)

│   │   ├── EventLoggingService.h    (Enregistre logs)

│   │   └── ConfigService.h          (Gère config)

│   │

│   └── src/

│       └── MainApp.cpp              (Boucle principale)

│

├── mid/                    ← COMMUNICATION

│   ├── ble/

│   │   └── BLEManager.h             (Bluetooth)

│   └── i2c/

│       └── I2CManager.h             (Bus I2C)

│

├── low/                    ← DRIVERS HARDWARE

│   ├── hal/

│   │   ├── sensors/

│   │   │   ├── INA237Driver.h    (INA237 measures current, voltage)

│   │   │   └── TempSensorDriver.h       (NTC)

|    |   |   └── tempsensor 2

│   │   ├── IHM/

│   │   │   ├── LED.h                    (Verte/Rouge)

│   │   │   ├── Buzzer.h                 (Son)

│   │   │   └── Button.h                 (Reset/Boot)

**│   │   ├── adc/**

**│   │   │   └── ADCDriver.h              (Analog)**

**│   │   ├── uart/**

**│   │   │   └── UARTDriver.h             (Serial)**

**│   │   └── i2c/**

**│   │       └── I2CHardware.h            (I2C bas-niveau)**

│   │

│   └── memory/

│       ├── PSRAMManager.h            (Logs)

│       ├── FlashStorage.h            (Config)

│       └── RingBuffer.h              (Rotation)

│

├── config/

│   ├── AppConfig.h         (Seuils, fréquences métier)

│   ├── HardwareConfig.h    (Pins GPIO, adresses I2C) à mettre dans les drv

│   └── SystemConfig.h      (Globaux) config gen de l'app

│

└── test/

&#x20;   ├── unit/               (Tests drivers individuels)

&#x20;   └── integration/        (Tests entre couches)





════════════════════════════════════════════════════════════════════════════

&#x20; RÈGLE D'OR

════════════════════════════════════════════════════════════════════════════



✅ AUTORISÉ:

&#x20; APP  →  MID  →  LOW  →  Hardware

&#x20; ↓      ↓      ↓

&#x20; OK     OK     OK



❌ INTERDIT:

&#x20; MID → APP

&#x20; LOW → MID ou APP

&#x20; Dépendances circulaires





════════════════════════════════════════════════════════════════════════════

&#x20; QUOI VA OÙ?

════════════════════════════════════════════════════════════════════════════



"Je dois mesurer le courant" → LOW (CurrentSensorDriver)

"Je dois allumer la LED" → LOW (LED)

"Je dois envoyer un message BLE" → MID (BLEManager)

"Je dois détecter une alarme" → APP (AlarmService)

"Je dois enregistrer en PSRAM" → LOW (PSRAMManager)

"Je dois gérer la boucle principale" → APP (MainApp)

"Je dois automatiser boot/reset" → MID (BootSequence)

"Je dois gérer la priorité USB>BT>HT" → LOW (PowerManager)





════════════════════════════════════════════════════════════════════════════

&#x20; EXEMPLE DE CODE

════════════════════════════════════════════════════════════════════════════



// ❌ MAUVAIS: Tout mélangé

void MainApp::run() {

&#x20;   while(true) {

&#x20;       digitalWrite(32, HIGH);                    // ← LOW

&#x20;       i2c\_write(0x40, 0x05);                     // ← LOW

&#x20;       ble\_send\_notification("courant", 25.0);   // ← MID

&#x20;       // ...

&#x20;   }

}



// ✅ BON: Séparation par couches

void MainApp::run() {

&#x20;   while(true) {

&#x20;       m\_monitoring.update();           // ← APP appelle le service

&#x20;       m\_alarm.checkAlarms();           // ← APP logique métier

&#x20;       delay(100);

&#x20;   }

}



void MonitoringService::update() {

&#x20;   float current = m\_sensor.readCurrent();  // ← APP utilise LOW

}



float CurrentSensorDriver::readCurrent() {

&#x20;   return i2c\_read(0x40, 0x05);  // ← LOW accède hardware

}





════════════════════════════════════════════════════════════════════════════

&#x20; COMMANDES BUILD (SIMPLE)

════════════════════════════════════════════════════════════════════════════



\# Créer dossiers

mkdir -p app/{services,src} mid/{ble,usb,i2c} low/hal/{sensors,gpio,adc,uart,i2c} low/memory



\# Compiler

cmake .

cmake --build .



\# Tests

ctest



\# Flash ESP32

esptool.py write\_flash 0x0000 build/ventec.bin





════════════════════════════════════════════════════════════════════════════

&#x20; RÉSUMÉ (1 PAGE)

════════════════════════════════════════════════════════════════════════════



┌─────────────────────────────────────────────┐

│ APP: "Je veux mesurer le courant et alarmer"│ ← Ici tu codes la logique

├─────────────────────────────────────────────┤

│ MID: "Je peux envoyer ça via BLE"           │ ← Communication

├─────────────────────────────────────────────┤

│ LOW: "Je peux lire l'INA237, allumer LED"   │ ← Hardware brut

├─────────────────────────────────────────────┤

│ Hardware: ESP32, INA237, LED, Antenne BLE   │ ← Puces physiques

└─────────────────────────────────────────────┘



Flux de données:

&#x20; Mesure → APP (logique) → MID (envoi) → Hardware (physique)

&#x20; 

Dépendances:

&#x20; APP dépend de MID ← MID dépend de LOW ← LOW dépend du Hardware

&#x20; 

Avantage:

&#x20; Changer un driver LOW = APP ne change pas

&#x20; Ajouter un service APP = LOW ne change pas





════════════════════════════════════════════════════════════════════════════

&#x20; C'EST TOUT! 🎉

════════════════════════════════════════════════════════════════════════════



Étapes pour commencer:



1\. Créer les dossiers (mkdir)

2\. Faire les fichiers .h dans inc/

3\. Faire les fichiers .cpp dans src/

4\. Compiler: cmake --build .

5\. Tester sur ESP32



Besoin d'aide sur un point spécifique? 🚀



