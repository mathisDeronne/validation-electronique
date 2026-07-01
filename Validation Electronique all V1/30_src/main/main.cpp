// ============================================================================
// FILE: main/main.cpp
// LAYER: Point d'entrée ESP-IDF
// PURPOSE: Lance la couche application (MainApp). Rien d'autre.
// ============================================================================

#include "MainApp.hpp"

// Objet applicatif statique (MISRA : pas de malloc).
static app::MainApp g_app;

// Point d'entrée imposé par ESP-IDF.
//
// NOTE : le `extern "C"` n'est PAS du code C — tout le projet est en C++.
// C'est l'unique endroit où le compilateur C++ doit exposer le symbole sous son
// nom non décoré, car le framework ESP-IDF (écrit en C) appelle cette fonction
// par le nom littéral "app_main". Obligatoire dans tout projet ESP32 en C++.
extern "C" void app_main(void)
{
    const common::Status result = g_app.init();

    if (result == common::Status::OK)
    {
        g_app.run();
    }
    else
    {
        // Même en cas d'échec d'init, on entre dans la FSM : elle basculera
        // vers un état d'erreur signalé par l'IHM.
        g_app.run();
    }
}
