// ============================================================================
// FILE: components/mid/ble_manager.cpp
// LAYER: MID (communication)
// STATUT: SQUELETTE
// ============================================================================

#include "ble_manager.hpp"

namespace mid {

BleManager::BleManager() : m_initialized(false), m_advertising(false) {}

common::Status BleManager::init()
{
    // TODO: nimble_port_init(), config GATT services/caractéristiques
    m_initialized = true;
    return common::Status::OK;
}

void BleManager::startAdvertising()
{
    if (m_initialized)
    {
        // TODO: ble_gap_adv_start(...)
        m_advertising = true;
    }
}

void BleManager::updateCurrent(int32_t current_ma)
{
    // TODO: ble_gatts_chr_updated(...) avec current_ma
    (void)current_ma;
}

void BleManager::updateTemperatures(common::float32_t pcb1, common::float32_t pcb2,
                                    common::float32_t ambient)
{
    (void)pcb1;
    (void)pcb2;
    (void)ambient;
}

void BleManager::notifyAlarm(common::AlarmLevel level)
{
    (void)level;
}

} // namespace mid
