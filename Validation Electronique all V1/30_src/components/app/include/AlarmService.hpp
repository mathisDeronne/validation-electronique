// ============================================================================
// FILE: components/app/include/AlarmService.hpp
// LAYER: APP (service métier)
// PURPOSE: Traduit un niveau d'alarme en commande IHM (LED/buzzer)
// REQUIS: EXF-24, EXF-25, EXF-27
// ============================================================================

#ifndef ALARM_SERVICE_HPP
#define ALARM_SERVICE_HPP

#include "common_types.hpp"
#include "drv_ihm.hpp"

namespace app {

class AlarmService {
public:
    explicit AlarmService(low::DrvIhm& ihm);

    common::Status init();

    // Applique la signalisation correspondant au niveau (EXF-24, EXF-25)
    void showLevel(common::AlarmLevel level);

    // Affiche directement un mode (succès, erreur…) pour la FSM
    void showMode(common::HmiMode mode);

    bool isResetPressed();

private:
    low::DrvIhm& m_ihm;
};

} // namespace app

#endif // ALARM_SERVICE_HPP
