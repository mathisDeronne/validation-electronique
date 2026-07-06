// ============================================================================
// FILE: main/main.cpp
// LAYER: Point d'entrée ESP-IDF
// ============================================================================

#include "MainApp.hpp"

static app::MainApp g_app;

// extern "C" obligatoire : ESP-IDF (en C) appelle app_main par son nom littéral.
extern "C" void app_main(void)
{
    const common::Status result = g_app.init();

    if (result == common::Status::OK)
    {
        g_app.run();
    }
    else
    {
        g_app.run(); // La FSM basculera vers un état d'erreur
    }
}
