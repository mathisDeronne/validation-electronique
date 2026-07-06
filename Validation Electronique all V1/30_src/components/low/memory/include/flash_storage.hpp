// ============================================================================
// FILE: components/low/memory/include/flash_storage.hpp
// LAYER: LOW (memory)
// PURPOSE: Configuration persistante (seuils modifiables - EXF-23)
// STATUT: SQUELETTE — encapsulera NVS (Non-Volatile Storage) ESP-IDF
// ============================================================================

#ifndef FLASH_STORAGE_HPP
#define FLASH_STORAGE_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class FlashStorage {
public:
    FlashStorage();
    common::Status init();
    common::Status writeInt32(const char* key, int32_t value);
    common::Status readInt32(const char* key, int32_t& out_value);
private:
    bool m_initialized;
};

} // namespace low

#endif // FLASH_STORAGE_HPP
