#ifndef TEST_ROCE_BRAMBUFFER_H
#define TEST_ROCE_BRAMBUFFER_H

#include "buffer_manager/IBuffer.h"
#include <unistd.h>
#include <cstdint>
#include <string>

class BRAMBuffer: public IBuffer<uint64_t>{
public:
    BRAMBuffer(std::size_t size, off_t offset, const std::string& device);
    ~BRAMBuffer() override;

    uint64_t * getAddress() const override;
    std::size_t getMaxSize() const override;
    std::size_t getAlignment() const override;
    void clear() override;
    ibv_mr * registerBuffer(ibv_pd* pd, unsigned int access) override;

    uint64_t operator [](uint32_t i) const override;
    uint64_t& operator [](uint32_t i) override;
    int getFileDescriptor() const;

private:
    const std::size_t MAX_BRAM_SIZE = 0x2000;
    const off_t BRAM_BASE_ADDR = 0xA0000000;
    int m_fd = -1;
    uint64_t * m_vptr;
    std::size_t m_allocated_size;
    ibv_context* m_ctx;

    ibv_context* createContext(const std::string& device_name);
};

#endif //TEST_ROCE_BRAMBUFFER_H
