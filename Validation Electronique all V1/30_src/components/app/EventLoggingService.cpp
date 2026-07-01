// ============================================================================
// FILE: components/app/EventLoggingService.cpp
// LAYER: APP (service métier)
// STATUT: SQUELETTE
// ============================================================================

#include "EventLoggingService.hpp"

namespace app {

EventLoggingService::EventLoggingService(low::PsramManager& psram)
    : m_psram(psram)
{
}

common::Status EventLoggingService::init()
{
    return m_psram.init();
}

void EventLoggingService::logMeasurement(int32_t current_ma, common::float32_t temp_c)
{
    low::LogEntry entry = { 0U, current_ma, temp_c, 0U };
    // TODO: horodatage réel (esp_timer_get_time)
    (void)m_psram.append(entry);
}

void EventLoggingService::logAlarm(common::AlarmLevel level, int32_t trigger_value)
{
    low::LogEntry entry = { 0U, trigger_value, 0.0F,
                            static_cast<uint8_t>(level) };
    (void)m_psram.append(entry);
}

void EventLoggingService::enforceRotation()
{
    // La rotation FIFO est gérée par le RingBuffer/PSRAM (EXF-18).
}

} // namespace app
