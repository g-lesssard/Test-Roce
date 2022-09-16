#ifndef TEST_ROCE_CIRCULARBUFFER_H
#define TEST_ROCE_CIRCULARBUFFER_H

#include "buffer_manager/ICircularBuffer.h"
#include "buffer_manager/IBuffer.h"

template<class type>
class CircularBuffer : public ICircularBuffer<type> {
public:
    CircularBuffer(IBuffer<type>& buffer);
    ~CircularBuffer() override = default;

    uint16_t getFreeSize() const override;
    uint16_t getDataLength() const  override;
    void clear() override;
    bool isFull() const override;
    CircularBufferResources<type> getResources() const override;

    bool putc(type data) override;
    bool put(type *data, uint16_t length) override;
    bool getc(type &data) override;
    uint16_t get(type *data, uint16_t length) override;

private:
    IBuffer<type>& m_buffer;
    CircularBufferResources<type> m_resources;
};

#include "buffer_manager/CircularBuffer.tpp"
#endif //TEST_ROCE_CIRCULARBUFFER_H
