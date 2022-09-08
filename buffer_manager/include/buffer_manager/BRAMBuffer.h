#ifndef TEST_ROCE_BRAMBUFFER_H
#define TEST_ROCE_BRAMBUFFER_H

#include "buffer_manager/IBuffer.h"
#include <unistd.h>
#include <cstdint>

class BRAMBuffer: public IBuffer {
public:
    BRAMBuffer(std::size_t size, off_t offset);
    ~BRAMBuffer() override;

    void * getAddress() const override;
    std::size_t getMaxSize() const override;
    std::size_t getAlignment() const override;
    void clear() override;

    uint64_t operator [](int i) const;
    uint64_t& operator [](int i);

private:
    const std::size_t MAX_BRAM_SIZE = 0x2000;
    const off_t BRAM_BASE_ADDR = 0xA0000000;
    int m_fd = -1;
    uint64_t * m_vptr;
    std::size_t m_allocated_size;
};

#endif //TEST_ROCE_BRAMBUFFER_H
