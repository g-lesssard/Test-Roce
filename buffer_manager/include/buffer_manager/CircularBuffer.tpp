#ifndef __CIRCULAR_BUFFER_TPP_
#define __CIRCULAR_BUFFER_TPP_

#include "buffer_manager/CircularBuffer.h"

template<class type>
CircularBuffer<type>::CircularBuffer(IBuffer<type>& buffer) : m_buffer(buffer) {
    m_resources.m_size = buffer.getMaxSize();
    m_resources.m_data = buffer.getAddress();
    m_resources.m_readPos = 0;
    m_resources.m_writePos = 0;
    m_resources.m_isFull = false;
}

template<class type>
uint16_t CircularBuffer<type>::getFreeSize() const {
    if (!m_resources.m_data) {
        return 0;
    }

    if (m_resources.m_isFull) {
        return 0;
    }

    return m_resources.m_size - getDataLength();
}

template<class type>
uint16_t CircularBuffer<type>::getDataLength() const {
    if (!m_resources.m_data) {
        return 0;
    }

    if (m_resources.m_isFull) {
        return m_resources.m_size;
    }

    if (m_resources.m_readPos > m_resources.m_writePos) {
        return m_resources.m_size - (m_resources.m_readPos - m_resources.m_writePos);
    }
    return m_resources.m_writePos - m_resources.m_readPos;
}

template<class type>
void CircularBuffer<type>::clear() {
    m_resources.m_readPos = 0;
    m_resources.m_writePos = 0;
    m_resources.m_isFull = false;
}

template<class type>
bool CircularBuffer<type>::isFull() const {
    return m_resources.m_isFull;
}

template<class type>
CircularBufferResources<type> CircularBuffer<type>::getResources() const {return m_resources;}

template<class type>
bool CircularBuffer<type>::putc(type data) {
    if (!m_resources.m_data || isFull()) {
        return false;
    }

    m_buffer[m_resources.m_writePos++] = data;

    if (m_resources.m_writePos >= m_resources.m_size) {
        m_resources.m_writePos = 0;
    }

    if (m_resources.m_writePos == m_resources.m_readPos) {
        m_resources.m_isFull = true;
    }

    return true;
}

template<class type>
bool CircularBuffer<type>::put(type *data, uint16_t length) {
    if (!m_resources.m_data || m_resources.m_isFull || length > getFreeSize()) {
        return false;
    }

    for (uint16_t i = 0; i < length; i++) {
        putc(data[i]);
    }

    return true;
}

template<class type>
bool CircularBuffer<type>::getc(type &data) {
    if (!m_resources.m_data || getDataLength() == 0) {
        return false;
    }

    data = m_buffer[m_resources.m_readPos++];

    if (m_resources.m_readPos >= m_resources.m_size) {
        m_resources.m_readPos = 0;
    }

    m_resources.m_isFull = false;
    return true;
}

template<class type>
uint16_t CircularBuffer<type>::get(type *data, uint16_t length) {
    if (!m_resources.m_data) {
        return false;
    }

    uint16_t buffDataSize = getDataLength();
    uint16_t copySize = length > buffDataSize ? buffDataSize : length;

    for (uint16_t i = 0; i < copySize; ++i) {
        getc(data[i]);
    }

    return copySize;
}

#endif //__CIRCULAR_BUFFER_TPP_
