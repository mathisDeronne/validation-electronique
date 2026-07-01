// ============================================================================
// FILE: components/app/include/EventLoggingService.hpp
// LAYER: APP (service métier)
// PURPOSE: Enregistre l'historique des mesures et alarmes (EXF-15 à EXF-18)
// STATUT: SQUELETTE — s'appuiera sur low::PsramManager + low::RingBuffer
// ============================================================================

#ifndef EVENT_LOGGING_SERVICE_HPP
#define EVENT_LOGGING_SERVICE_HPP

#include <cstdint>
#include "common_types.hpp"
#include "psram_manager.hpp"

namespace app {

class EventLoggingService {
public:
    explicit EventLoggingService(low::PsramManager& psram);

    common::Status init();
    // Enregistre une mesure horodatée (EXF-15)
    void logMeasurement(int32_t current_ma, common::float32_t temp_c);
    // Enregistre une alarme avec sa valeur de déclenchement (EXF-16)
    void logAlarm(common::AlarmLevel level, int32_t trigger_value);
    // Politique de rotation FIFO (EXF-18) — déléguée au RingBuffer/PSRAM
    void enforceRotation();

private:
    low::PsramManager& m_psram;
};

} // namespace app

#endif // EVENT_LOGGING_SERVICE_HPP
