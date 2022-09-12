#include "buffer_manager/BRAMBuffer.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <type_traits>
#include <cstring>
#include <algorithm>

BRAMBuffer::BRAMBuffer(std::size_t size, off_t offset) {
    m_allocated_size = std::min(size, MAX_BRAM_SIZE - offset);
    m_fd = open("/dev/mem", O_RDWR | O_SYNC);
    m_vptr = (uint64_t *) mmap(NULL, m_allocated_size, PROT_READ | PROT_WRITE,
                                           MAP_SHARED, m_fd, BRAM_BASE_ADDR + offset);
}

BRAMBuffer::~BRAMBuffer() {
    munmap(m_vptr, m_allocated_size);
    close(m_fd);
}

void *BRAMBuffer::getAddress() const { return m_vptr;}

std::size_t BRAMBuffer::getMaxSize() const { return m_allocated_size;}

std::size_t BRAMBuffer::getAlignment() const {return std::alignment_of_v<uint64_t>;}

void BRAMBuffer::clear() {
    memset(m_vptr, 0, m_allocated_size);
}

uint64_t BRAMBuffer::operator[](int i) const {
    return m_vptr[i];
}

uint64_t& BRAMBuffer::operator[](int i) {
    return m_vptr[i];
}

int BRAMBuffer::getFileDescriptor() const { return m_fd;}