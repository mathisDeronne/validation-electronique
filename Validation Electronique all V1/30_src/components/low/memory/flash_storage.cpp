// ============================================================================
// FILE: components/low/memory/flash_storage.cpp
// LAYER: LOW (memory)
// STATUT: SQUELETTE
// ============================================================================

#include "flash_storage.hpp"

// Sur cible : #include "nvs_flash.h" / #include "nvs.h"

namespace low {

FlashStorage::FlashStorage() : m_initialized(false) {}

common::Status FlashStorage::init()
{
    // TODO: nvs_flash_init(); nvs_open(...)
    m_initialized = true;
    return common::Status::OK;
}

common::Status FlashStorage::writeInt32(const char* key, int32_t value)
{
    common::Status result = common::Status::OK;
    if ((key == nullptr) || (!m_initialized))
    {
        result = common::Status::ERROR_NULL_POINTER;
    }
    else
    {
        // nvs_set_i32(...); nvs_commit(...);
        (void)value;
    }
    return result;
}

common::Status FlashStorage::readInt32(const char* key, int32_t& out_value)
{
    common::Status result = common::Status::OK;
    if ((key == nullptr) || (!m_initialized))
    {
        result = common::Status::ERROR_NULL_POINTER;
    }
    else
    {
        // nvs_get_i32(...);
        out_value = 0;
    }
    return result;
}

} // namespace low
