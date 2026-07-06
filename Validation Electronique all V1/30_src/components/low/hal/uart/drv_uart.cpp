// ============================================================================
// FILE: components/low/hal/uart/drv_uart.cpp
// LAYER: LOW (HAL - UART)
// STATUT: SQUELETTE
// ============================================================================

#include "drv_uart.hpp"

// Sur cible : #include "driver/uart.h"

namespace low {

DrvUart::DrvUart() : m_initialized(false) {}

common::Status DrvUart::init(uint32_t baudrate)
{
    // TODO: uart_param_config(...), uart_driver_install(...)
    (void)baudrate;
    m_initialized = true;
    return common::Status::OK;
}

common::Status DrvUart::write(const uint8_t* data, uint16_t length)
{
    common::Status result = common::Status::OK;
    if ((data == nullptr) || (!m_initialized))
    {
        result = common::Status::ERROR_NULL_POINTER;
    }
    else
    {
        // uart_write_bytes(...);
        (void)length;
    }
    return result;
}

uint16_t DrvUart::read(uint8_t* buffer, uint16_t max_length)
{
    uint16_t received = 0U;
    if ((buffer != nullptr) && m_initialized)
    {
        // received = uart_read_bytes(...);
        (void)max_length;
    }
    return received;
}

} // namespace low
