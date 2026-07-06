La carte doit respecter les exigences suivantes :
- alimentation par trois sources : USB-C 5 V, BT entre 2,7 V et 10 V, HT jusqu’à 36 V ;
- priorité matérielle des alimentations : USB > BT > HT ;
- une seule source active à la fois ;
- conversion de chaque source vers un rail logique 3,3 V ;
- absence de retour de courant entre les sources ;
- mesure du courant batterie avec un INA237 ;
- mesure de température avec deux NTC PCB et un capteur numérique TMP126 ;
- communication BLE pour consulter les données ;
- stockage des logs : mesures, alarmes, reset, changements d’alimentation, connexions technicien ;
- alertes locales avec LED verte, LED rouge et buzzer ;
- présence de connecteurs Grove et MikroBus ;
- programmation et extraction des logs via USB-C.

Composants principaux de la carte :
- ESP32-PICO-D4 : microcontrôleur principal ;
- CP2104 : interface USB-UART ;
- USB-C : alimentation, programmation et extraction des logs ;
- INA237 : mesure du courant ;
- TMP126 : mesure numérique de température ;
- NTC : mesure de température PCB ;
- LED, buzzer, antenne BLE, boutons boot/reset ;
- connecteurs Grove et MikroBus.

Procédure de test proposée

1. Vérifications avant alimentation

Avant de mettre la carte sous tension, il faut faire une inspection visuelle complète :
- vérifier le sens des composants : ESP32, CP2104, INA237, TMP126, régulateurs, MOSFETs ;
- vérifier qu’il n’y a pas de court-circuit entre 3V3 et GND ;
- vérifier qu’il n’y a pas de court-circuit entre VBUS et GND ;
- vérifier qu’il n’y a pas de court-circuit entre HT et GND ;
- contrôler les soudures, les ponts éventuels et les composants manquants ;
- vérifier que l’antenne BLE est bien dégagée et que le plan de masse ne gêne pas son fonctionnement.

Cette étape est indispensable pour éviter de détruire un composant au premier démarrage.

2. Test des alimentations

La validation doit commencer par les alimentations, car c’est le bloc le plus critique.

Objectif du test :
Vérifier que chaque source d’alimentation permet bien de fournir un rail logique 3,3 V stable à la carte.

Procédure de test :
- utiliser une alimentation de laboratoire avec limitation de courant ;
- commencer avec une limite de courant faible pour éviter de détruire la carte en cas de défaut ;
- alimenter une seule source à la fois ;
- mesurer la tension d’entrée de la source utilisée ;
- mesurer la sortie du convertisseur correspondant ;
- mesurer le rail logique 3,3 V ;
- vérifier que l’ESP32 démarre correctement ;
- contrôler qu’aucun composant ne chauffe anormalement.

Test USB :
- brancher uniquement l’USB-C ;
- mesurer VBUS, qui doit être autour de 5 V ;
- mesurer le rail 3,3 V ;
- vérifier que l’ESP32 démarre ;
- vérifier que le port série apparaît sur le PC ;
- flasher le programme avec PlatformIO.

Test BT :
- débrancher USB et HT ;
- alimenter l’entrée BT avec une alimentation de laboratoire ;
- tester plusieurs tensions : 2,7 V, 3,7 V, 5 V et 10 V ;
- vérifier que le rail 3,3 V reste stable ;
- vérifier que la carte démarre correctement.

Test HT :
- débrancher USB et BT ;
- alimenter l’entrée HT progressivement ;
- tester 12 V, 24 V puis 36 V ;
- vérifier la sortie du convertisseur HT ;
- vérifier que le rail 3,3 V reste stable ;
- contrôler qu’aucun composant ne chauffe anormalement.

Critères de validation :
- le rail 3,3 V doit rester stable pour chaque source ;
- la carte doit démarrer quelle que soit la source utilisée ;
- aucun composant ne doit chauffer de manière anormale ;
- la consommation doit rester cohérente avec une carte ESP32 alimentée.

3. Test de priorité des alimentations

Objectif du test :
Vérifier que la priorité matérielle USB > BT > HT est bien respectée, et qu’une seule source alimente réellement la carte à la fois.

Procédure de test :
- brancher d’abord uniquement la source HT ;
- vérifier que la carte fonctionne avec HT ;
- ajouter ensuite la source BT sans retirer HT ;
- vérifier que la carte bascule sur BT ;
- ajouter ensuite l’USB sans retirer BT ni HT ;
- vérifier que la carte bascule sur USB ;
- retirer l’USB ;
- vérifier que la carte revient sur BT ;
- retirer BT ;
- vérifier que la carte revient sur HT.

Pendant le test, il faut mesurer :
- le rail 3,3 V ;
- les sorties des convertisseurs USB, BT et HT ;
- les tensions présentes sur les sources non utilisées ;
- éventuellement les transitoires à l’oscilloscope.

Critères de validation :
- HT seule : source active = HT ;
- HT + BT : source active = BT ;
- HT + BT + USB : source active = USB ;
- retrait USB : retour sur BT ;
- retrait BT : retour sur HT ;
- aucune chute du 3,3 V ne doit provoquer un reset de l’ESP32 ;
- il ne doit pas y avoir de retour de courant vers une source inactive.

4. Test de l’ESP32, du reset et du boot

Objectif du test :
Vérifier que le microcontrôleur démarre, peut être reprogrammé et peut être redémarré matériellement.

Procédure de test :
- alimenter la carte par USB ;
- ouvrir PlatformIO ;
- lancer une compilation du programme ;
- flasher le firmware ;
- ouvrir le moniteur série ;
- vérifier que les messages de démarrage apparaissent ;
- appuyer sur le bouton RESET ;
- vérifier que la carte redémarre ;
- maintenir BOOT puis appuyer sur RESET pour tester le mode programmation ;
- refaire un flash si nécessaire.

Critères de validation :
- le port série USB est détecté ;
- le firmware peut être envoyé dans l’ESP32 ;
- le moniteur série affiche les logs de démarrage ;
- le bouton RESET redémarre bien la carte ;
- le bouton BOOT permet bien d’entrer en mode flash.

Si le flash ne fonctionne pas, il faut contrôler :
- le CP2104 ;
- les lignes TX/RX ;
- les signaux EN et GPIO0 ;
- le câblage USB ;
- le driver USB sur le PC.

5. Test de l’INA237

L’INA237 sert à mesurer le courant consommé sur la batterie 36 V.

Objectif du test :
Vérifier que l’INA237 communique correctement avec l’ESP32 et que la mesure de courant est cohérente.

Procédure de test :
- alimenter la carte ;
- lancer un scan I2C depuis le firmware ;
- vérifier que l’adresse de l’INA237 est détectée ;
- lire un registre de configuration ou d’identification ;
- afficher les valeurs mesurées dans le moniteur série ou via BLE ;
- simuler plusieurs courants à l’aide des points de test, d’une résistance de shunt ou d’une charge connue ;
- comparer la valeur affichée par le firmware avec la valeur attendue.

Exemples de points de mesure :
- 0 A ;
- 2 A ;
- 5 A ;
- 10 A ;
- 15 A ;
- 20 A ;
- 25 A ;
- 30 A.

Il faut aussi vérifier le sens de mesure :
- si le courant est positif dans le bon sens, le câblage IN+ / IN− est correct ;
- si le courant est négatif alors qu’il devrait être positif, les entrées IN+ et IN− sont probablement inversées ;
- si la valeur reste bloquée à zéro, il faut vérifier la communication I2C, le shunt et la configuration logicielle.

Critères de validation :
- l’INA237 répond sur le bus I2C ;
- les mesures évoluent quand le courant varie ;
- la valeur mesurée reste proche de la valeur attendue ;
- le signe du courant est correct ;
- le seuil d’alarme courant se déclenche correctement.

6. Test des températures NTC

Les deux NTC servent à mesurer la température du PCB.

Objectif du test :
Vérifier que les deux NTC sont bien lues par l’ESP32 et que les températures indiquées sont cohérentes avec la température réelle.

Procédure de test :
- placer la carte dans un environnement stable, sans source de chaleur proche ;
- attendre quelques minutes que la carte soit à température ambiante ;
- mesurer la température ambiante avec un thermomètre de référence placé proche de la carte ;
- lire les valeurs NTC dans le moniteur série ou via BLE ;
- comparer les valeurs lues avec la température de référence ;
- chauffer légèrement la zone de la première NTC ;
- vérifier que la température NTC1 augmente ;
- laisser refroidir ;
- refaire le même test avec NTC2 ;
- vérifier que les deux mesures reviennent vers la température ambiante.

Pour s’assurer que la température est correcte :
- ne pas se contenter de voir que la valeur change ;
- comparer la température mesurée avec un thermomètre externe ;
- faire plusieurs mesures et prendre une moyenne ;
- vérifier l’écart entre la température affichée et la température réelle.

Exemple :
Si le thermomètre de référence indique 21 °C, les NTC ne doivent pas afficher 30 °C sans raison. Un petit écart est acceptable, car les NTC mesurent la température du PCB et non exactement l’air ambiant. Par contre, un écart constant de 5 à 10 °C doit être analysé.

Critères de validation :
- à température ambiante, les valeurs doivent être cohérentes avec le thermomètre de référence ;
- les deux NTC doivent réagir à la chauffe ;
- les deux NTC doivent redescendre après refroidissement ;
- le seuil d’alarme température doit se déclencher autour de la valeur définie, par exemple 75 °C ;
- les valeurs ne doivent pas être instables ou bruitées.

Si la mesure est incorrecte, il faut vérifier :
- la valeur de la NTC ;
- la résistance du diviseur ;
- la formule de conversion utilisée dans le firmware ;
- la configuration ADC de l’ESP32 ;
- la tension de référence utilisée pour le calcul ;
- le filtrage logiciel ou matériel.

7. Test du TMP126

Le TMP126 est le capteur numérique de température ambiante. C’est un point important du projet, car un problème de communication SPI avait été identifié.

Objectif du test :
Vérifier que le TMP126 communique correctement avec l’ESP32 et qu’il renvoie une température cohérente avec la température réelle.

Procédure de test :
- placer la carte à température ambiante stable ;
- placer un thermomètre de référence à proximité du TMP126 ;
- attendre quelques minutes que la température soit stabilisée ;
- lire la température TMP126 dans le moniteur série ou via BLE ;
- comparer la valeur TMP126 avec le thermomètre de référence ;
- faire plusieurs lectures successives et vérifier que la valeur reste stable ;
- chauffer légèrement l’air autour du capteur ou approcher une source chaude modérée ;
- vérifier que la température TMP126 augmente progressivement ;
- laisser refroidir et vérifier que la température redescend.

Pour valider la communication SPI, utiliser un analyseur logique sur :
- CS ;
- SCK ;
- SIO / MOSI / MISO selon le câblage ;
- GND.

Il faut vérifier que :
- le CS est bien activé pendant la communication ;
- l’horloge SCK est présente ;
- la bonne commande de lecture est envoyée ;
- le capteur répond avec une valeur différente de 0x0000 ou 0xFFFF ;
- la température mesurée est cohérente avec la température ambiante.

Point important :
Le TMP126 attend une commande de lecture avant d’envoyer sa donnée. Pour lire le registre température à l’adresse 0x00, il ne faut pas envoyer 0x0000, car cela correspond à une écriture ou à une commande incorrecte. Il faut envoyer une commande de lecture, typiquement 0x0100, avec le bit read/write à 1. Sinon le capteur ne pilote pas la ligne SIO et la lecture retourne souvent 0x0000 ou 0xFFFF.

Pour s’assurer que la température est correcte :
- comparer avec un thermomètre externe ;
- vérifier la stabilité de la mesure sur plusieurs secondes ;
- vérifier que la valeur augmente quand on chauffe ;
- vérifier que la valeur redescend quand on refroidit ;
- comparer aussi avec les NTC, même si les valeurs ne seront pas forcément identiques.

Exemple :
Si la température ambiante réelle est de 21 °C, le TMP126 doit afficher une valeur proche de 21 °C. Une valeur de 24 °C peut être acceptable uniquement si la carte ou le capteur est légèrement chauffé par les composants proches. Si l’écart reste constant à froid, il faut corriger la conversion ou appliquer un offset logiciel.

Critères de validation :
- la communication SPI est visible et correcte ;
- la commande de lecture est bien envoyée ;
- la valeur ne reste pas bloquée à 0x0000 ou 0xFFFF ;
- la température est proche de la température réelle ;
- la variation de température est logique lors d’une chauffe ou d’un refroidissement.

8. Test BLE

La carte doit communiquer en BLE pour permettre au technicien de lire les données sans ouvrir ou immobiliser longtemps l’AGV.

Objectif du test :
Vérifier que la carte est visible en BLE, que la connexion est stable et que les données sont correctement transmises.

Procédure de test :
- alimenter la carte ;
- scanner les périphériques BLE depuis un téléphone ou un PC ;
- vérifier que la carte apparaît avec le bon nom ;
- se connecter à la carte ;
- lire les caractéristiques GATT disponibles ;
- vérifier que les valeurs affichées correspondent aux mesures internes ;
- déclencher une variation de courant ou de température ;
- vérifier que les valeurs BLE sont mises à jour ;
- déclencher une alarme ;
- vérifier que l’alarme est visible en BLE ou envoyée par notification.

Données à vérifier :
- courant mesuré ;
- température NTC1 ;
- température NTC2 ;
- température TMP126 ;
- état d’alarme ;
- derniers logs ;
- source d’alimentation active si disponible.

Critères de validation :
- la carte est visible au scan BLE ;
- la connexion est possible ;
- les caractéristiques BLE sont lisibles ;
- les valeurs transmises sont cohérentes ;
- les notifications fonctionnent ;
- la carte ne redémarre pas en boucle lors d’une connexion BLE.

Si le BLE provoque des redémarrages, il faut regarder :
- l’initialisation BLE, qui ne doit pas être relancée en continu ;
- la mémoire disponible de l’ESP32 ;
- les callbacks BLE ;
- la fréquence d’envoi des notifications ;
- les conflits éventuels avec les autres tâches du programme.

9. Test des logs

Les logs doivent permettre de garder un historique des événements importants.

Objectif du test :
Vérifier que les mesures et événements sont bien enregistrés, horodatés et récupérables.

Procédure de test :
- alimenter la carte ;
- laisser tourner le programme quelques minutes ;
- vérifier que les mesures périodiques sont enregistrées ;
- déclencher une alarme courant ;
- déclencher une alarme température ;
- faire un reset ;
- changer de source d’alimentation ;
- se connecter en BLE ;
- extraire les logs par USB ou BLE ;
- vérifier que chaque événement apparaît dans les logs.

Les logs doivent contenir au minimum :
- un timestamp ou temps depuis démarrage ;
- le type d’événement ;
- la valeur du courant ;
- les températures ;
- la source d’alimentation active ;
- l’état d’alarme.

Exemple de format de log :
timestamp ; type_evenement ; courant ; temperature_NTC1 ; temperature_NTC2 ; temperature_TMP126 ; source_alim ; etat_alarme

Critères de validation :
- les mesures sont enregistrées régulièrement ;
- les alarmes sont enregistrées avec leur valeur de déclenchement ;
- les reset sont enregistrés ;
- les changements d’alimentation sont enregistrés ;
- les connexions BLE ou USB sont enregistrées ;
- les logs peuvent être extraits correctement ;
- la mémoire ne bloque pas le programme quand elle est pleine.

10. Test LED et buzzer

La carte doit indiquer localement son état.

Objectif du test :
Vérifier que les alertes locales correspondent bien à l’état réel du système.

Procédure de test :
- alimenter la carte en état normal ;
- vérifier que la LED verte est allumée ;
- vérifier que la LED rouge est éteinte ;
- vérifier que le buzzer est éteint ;
- simuler une alarme courant ;
- vérifier que la LED rouge s’active ;
- vérifier que le buzzer s’active ;
- vérifier que l’événement est enregistré dans les logs ;
- vérifier que l’information est transmise en BLE ;
- refaire le test avec une alarme température ;
- supprimer la condition d’alarme ;
- vérifier le retour à l’état normal.

État normal :
- LED verte allumée ;
- LED rouge éteinte ;
- buzzer éteint.

État alarme :
- LED rouge active ;
- buzzer actif ;
- événement enregistré dans les logs ;
- notification BLE envoyée.

Critères de validation :
- les LED correspondent au bon état ;
- le buzzer se déclenche uniquement en alarme ;
- les alarmes locales sont cohérentes avec les logs et le BLE ;
- le retour à l’état normal fonctionne.

11. Test Grove et MikroBus

Les connecteurs d’extension doivent être vérifiés.

Objectif du test :
Vérifier que les connecteurs Grove et MikroBus sont correctement alimentés et que les bus de communication sont fonctionnels.

Procédure de test Grove :
- mesurer le 3,3 V sur le connecteur ;
- vérifier la continuité du GND ;
- vérifier la présence des lignes SDA et SCL ;
- brancher un capteur I2C Grove compatible 3,3 V ;
- lancer un scan I2C ;
- vérifier que l’adresse du capteur est détectée ;
- vérifier qu’il n’y a pas de conflit avec l’INA237 ou d’autres composants I2C.

Procédure de test MikroBus :
- mesurer le 3,3 V et le GND ;
- vérifier les lignes SPI : MOSI, MISO, SCK, CS ;
- brancher un module MikroBus compatible si disponible ;
- lancer une communication simple ;
- observer les signaux à l’analyseur logique si nécessaire.

Critères de validation :
- les connecteurs sont alimentés en 3,3 V ;
- les masses sont bien communes ;
- les bus I2C et SPI fonctionnent ;
- les niveaux logiques sont compatibles avec l’ESP32 ;
- aucune adresse I2C ne rentre en conflit avec les composants déjà présents.

Risques principaux identifiés

Les points à surveiller en priorité sont :
- mauvaise priorité d’alimentation ou retour de courant entre sources ;
- chute du 3,3 V lors d’un changement de source ;
- TMP126 qui ne répond pas à cause d’une mauvaise commande SPI ;
- TMP126 ou NTC qui renvoient une température décalée par rapport à la réalité ;
- INA237 mal configuré ou shunt inversé ;
- BLE instable ou redémarrage en boucle ;
- logs non enregistrés ou non horodatés ;
- alarme LED/buzzer non synchronisée avec les logs et le BLE.

Conclusion

La validation de la carte doit se faire progressivement.

Il faut d’abord valider le hardware :
- alimentation USB, BT et HT ;
- priorité matérielle ;
- rail 3,3 V stable ;
- absence de retour de courant ;
- reset et boot.

Ensuite il faut valider les mesures :
- courant avec INA237 ;
- températures avec NTC ;
- température numérique avec TMP126.

Enfin il faut valider les fonctions applicatives :
- BLE ;
- logs ;
- alertes locales ;
- connecteurs Grove et MikroBus.

Le point le plus critique est la gestion d’alimentation, car elle doit fonctionner matériellement, sans dépendre du programme. Le second point critique est le TMP126, car une mauvaise commande SPI peut empêcher toute lecture correcte de température.

Pour les capteurs de température, il ne faut pas seulement vérifier qu’une valeur s’affiche. Il faut comparer la valeur mesurée avec une référence externe, comme un thermomètre placé à proximité de la carte, puis vérifier que l’écart reste raisonnable. Une mesure qui affiche 24 °C alors que l’environnement est à 21 °C doit être justifiée par l’échauffement de la carte ou corrigée dans le firmware.
```
