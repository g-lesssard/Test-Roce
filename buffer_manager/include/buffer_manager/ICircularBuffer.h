#ifndef TEST_ROCE_ICIRCULARBUFFER_H
#define TEST_ROCE_ICIRCULARBUFFER_H

#include <cstdint>

template<class type>
struct CircularBufferResources {
    type * m_data;
    uint16_t m_size;
    uint16_t m_readPos;
    uint16_t m_writePos;
    bool m_isFull;
};

template<class type>
class ICircularBuffer {
  public:
    virtual ~ICircularBuffer();

    virtual uint16_t getFreeSize() const  = 0;
    virtual uint16_t getDataLength() const = 0;
    virtual void clear() = 0;
    virtual bool isFull() const = 0;
    virtual CircularBufferResources<type> getResources() const = 0;
    virtual bool putc(uint8_t data) = 0;
    virtual bool put(uint8_t* data, uint16_t length) = 0;
    virtual bool getc(uint8_t& data) = 0;
    virtual uint16_t get(uint8_t* data, uint16_t length) = 0;

};

#endif //TEST_ROCE_ICIRCULARBUFFER_H
