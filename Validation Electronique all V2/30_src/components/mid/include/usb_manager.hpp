// ============================================================================
// FILE: components/mid/include/usb_manager.hpp
// LAYER: MID (communication)
// PURPOSE: Communication USB-C - extraction logs + MAJ firmware (EXF-21, EXF-34)
// STATUT: SQUELETTE — encapsulera TinyUSB CDC + DrvUart
// ============================================================================

#ifndef USB_MANAGER_HPP
#define USB_MANAGER_HPP

#include <cstdint>
#include "common_types.hpp"

namespace mid {

class UsbManager {
public:
    UsbManager();
    common::Status init();
    // Traite les commandes USB entrantes (non bloquant)
    void processIncomingCommands();
private:
    bool m_initialized;
};

} // namespace mid

#endif // USB_MANAGER_HPP
