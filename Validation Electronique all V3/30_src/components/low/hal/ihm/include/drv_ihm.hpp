// ============================================================================
// FILE: components/low/hal/ihm/include/drv_ihm.hpp
// LAYER: LOW (HAL - IHM)
// PURPOSE: Pilotage LED + buzzer (GPIO) et lecture boutons reset/boot
// REQUIS: EXF-24, EXF-25, EXF-26, EXF-33
// ============================================================================

#ifndef DRV_IHM_HPP
#define DRV_IHM_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class DrvIhm {
public:
    DrvIhm();

    common::Status init();
    void setNotification(common::HmiMode mode);
    bool isResetPressed();
    bool isBootPressed();

private:
    bool m_initialized;

    void writeLedGreen(bool on);
    void writeLedRed(bool on);
    void writeBuzzer(bool on);
};

} // namespace low

#endif // DRV_IHM_HPP
