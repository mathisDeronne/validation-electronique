// ============================================================================
// FILE: components/mid/usb_manager.cpp
// LAYER: MID (communication)
// STATUT: SQUELETTE
// ============================================================================

#include "usb_manager.hpp"

namespace mid {

UsbManager::UsbManager() : m_initialized(false) {}

common::Status UsbManager::init()
{
    // TODO: tinyusb_driver_install(...), config CDC-ACM
    m_initialized = true;
    return common::Status::OK;
}

void UsbManager::processIncomingCommands()
{
    if (m_initialized)
    {
        // TODO: lire les octets CDC, parser les commandes (logs, firmware)
    }
}

} // namespace mid
