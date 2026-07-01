// ============================================================================
// FILE: components/low/memory/include/psram_manager.hpp
// LAYER: LOW (memory)
// PURPOSE: Stockage des logs horodatés en PSRAM (EXF-15, EXF-16, EXF-17)
// STATUT: SQUELETTE — encapsulera esp_heap_caps / PSRAM
// ============================================================================

#ifndef PSRAM_MANAGER_HPP
#define PSRAM_MANAGER_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

// Une entrée de log horodatée (EXF-16)
struct LogEntry {
    uint32_t timestamp_ms;
    int32_t  current_ma;
    common::float32_t temp_c;
    uint8_t  event_type;
};

class PsramManager {
public:
    PsramManager();
    common::Status init();
    // Ajoute une entrée (rotation FIFO si plein - EXF-18)
    common::Status append(const LogEntry& entry);
    uint32_t getEntryCount() const;
private:
    bool     m_initialized;
    uint32_t m_count;
};

} // namespace low

#endif // PSRAM_MANAGER_HPP
