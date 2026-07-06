// ============================================================================
// FILE: components/low/memory/include/ring_buffer.hpp
// LAYER: LOW (memory)
// PURPOSE: Buffer circulaire pour la rotation des logs (EXF-18)
// MISRA: pas d'allocation dynamique, taille fixe par template
// ============================================================================

#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <cstdint>
#include <cstddef>

namespace low {

// Buffer circulaire à taille fixe (FIFO avec écrasement du plus ancien).
// Capacity est fixé à la compilation (MISRA : pas de malloc).
template <typename T, size_t Capacity>
class RingBuffer {
public:
    RingBuffer()
        : m_head(0U), m_count(0U)
    {
    }

    // Ajoute un élément. Si plein, écrase le plus ancien (EXF-18).
    void push(const T& item)
    {
        m_data[m_head] = item;
        m_head = (m_head + 1U) % Capacity;
        if (m_count < Capacity)
        {
            m_count++;
        }
    }

    size_t size() const  { return m_count; }
    bool   isEmpty() const { return m_count == 0U; }
    bool   isFull() const  { return m_count == Capacity; }
    size_t capacity() const { return Capacity; }

private:
    T      m_data[Capacity];
    size_t m_head;
    size_t m_count;
};

} // namespace low

#endif // RING_BUFFER_HPP
