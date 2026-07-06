// ============================================================================
// FILE: components/low/memory/psram_manager.cpp
// LAYER: LOW (memory)
// STATUT: SQUELETTE
// ============================================================================

#include "psram_manager.hpp"

namespace low {

PsramManager::PsramManager() : m_initialized(false), m_count(0U) {}

common::Status PsramManager::init()
{
    // TODO: heap_caps_malloc(..., MALLOC_CAP_SPIRAM) ou zone statique PSRAM
    m_initialized = true;
    return common::Status::OK;
}

common::Status PsramManager::append(const LogEntry& entry)
{
    common::Status result = common::Status::OK;
    if (!m_initialized)
    {
        result = common::Status::ERROR_INIT;
    }
    else
    {
        // TODO: écrire l'entrée en PSRAM avec rotation FIFO
        (void)entry;
        m_count++;
    }
    return result;
}

uint32_t PsramManager::getEntryCount() const
{
    return m_count;
}

} // namespace low
