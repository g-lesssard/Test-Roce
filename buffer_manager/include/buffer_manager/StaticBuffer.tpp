#include "buffer_manager/StaticBuffer.h"

template<class type, std::size_t size>
StaticBuffer<type, size>::StaticBuffer() {
    m_array = {};
}

template<class type, std::size_t size>
void *StaticBuffer<type, size>::getAddress() const {
    return const_cast<type*>(m_array.data());
}

template<class type, std::size_t size>
std::size_t StaticBuffer<type, size>::getMaxSize() const {
    return m_array.size();
}

template<class type, std::size_t size>
std::size_t StaticBuffer<type, size>::getAlignment() const {
    return std::alignment_of_v<type>;
}

template<class type, std::size_t size>
void StaticBuffer<type, size>::clear() {
    m_array.fill(0);
}

template<class type, std::size_t size>
std::array<type, size>&  StaticBuffer<type, size>::getContainer() {
    return m_array;
}


