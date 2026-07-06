// ============================================================================
// FILE: components/low/hal/uart/include/drv_uart.hpp
// LAYER: LOW (HAL - UART)
// PURPOSE: Communication série (CP2104) - reprogrammation (EXF-21, EXF-22)
// STATUT: SQUELETTE — encapsulera driver/uart.h
// ============================================================================

#ifndef DRV_UART_HPP
#define DRV_UART_HPP

#include <cstdint>
#include "common_types.hpp"

namespace low {

class DrvUart {
public:
    DrvUart();
    common::Status init(uint32_t baudrate);
    common::Status write(const uint8_t* data, uint16_t length);
    uint16_t read(uint8_t* buffer, uint16_t max_length);
private:
    bool m_initialized;
};

} // namespace low

#endif // DRV_UART_HPP
