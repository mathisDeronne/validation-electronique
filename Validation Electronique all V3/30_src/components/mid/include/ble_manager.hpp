// ============================================================================
// FILE: components/mid/include/ble_manager.hpp
// LAYER: MID (communication)
// PURPOSE: Communication BLE - API temps réel + seuils (EXF-19, EXF-20, EXF-23)
// STATUT: SQUELETTE — encapsulera le stack BLE de l'ESP32 (NimBLE)
// ============================================================================

#ifndef BLE_MANAGER_HPP
#define BLE_MANAGER_HPP

#include <cstdint>
#include "common_types.hpp"

namespace mid {

class BleManager {
public:
    BleManager();
    common::Status init();
    void startAdvertising();

    // Met à jour les caractéristiques GATT (EXF-19)
    void updateCurrent(int32_t current_ma);
    void updateTemperatures(common::float32_t pcb1, common::float32_t pcb2,
                            common::float32_t ambient);
    void notifyAlarm(common::AlarmLevel level);

private:
    bool m_initialized;
    bool m_advertising;
};

} // namespace mid

#endif // BLE_MANAGER_HPP
