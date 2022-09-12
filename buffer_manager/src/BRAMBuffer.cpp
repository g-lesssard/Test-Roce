#include "buffer_manager/BRAMBuffer.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <type_traits>
#include <cstring>
#include <algorithm>
#include <iostream>

BRAMBuffer::BRAMBuffer(std::size_t size, off_t offset, const std::string& device_name) {
    m_allocated_size = std::min(size, MAX_BRAM_SIZE - offset);
    m_fd = open("/dev/mem", O_RDWR | O_SYNC);
    m_vptr = (uint64_t *) mmap(NULL, m_allocated_size, PROT_READ | PROT_WRITE,
                                           MAP_SHARED, m_fd, BRAM_BASE_ADDR + offset);
    m_ctx = createContext(device_name);
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

ibv_mr *BRAMBuffer::registerBuffer(ibv_pd* pd, unsigned int access) {
    ibv_alloc_dm_attr dm_attr;
    dm_attr.length = getMaxSize();
    dm_attr.log_align_req = 3;
    auto* dm = ibv_alloc_dm(m_ctx, &dm_attr);
    if (!dm) {
        std::cerr << "BRAMBuffer: Failed to allocate device memory (" << std::strerror(errno) << ")" << std::endl;
        return nullptr;
    }
    ibv_mr* mr = ibv_reg_dm_mr(pd,
                               dm,
                               0,
                               getMaxSize(),
                               access);
    if (!mr) {
        std::cerr << "BRAMBuffer: Failed to register device memory (" << std::strerror(errno) << ")" << std::endl;
        return nullptr;
    }
    return mr;
    /*auto * mr2 = ibv_reg_dmabuf_mr(pd, 0, getMaxSize(), (uint64_t)getAddress(), getFileDescriptor(),access);
    if (!mr2) {
        std::cerr << "BRAMBuffer: Failed to register device memory (" << std::strerror(errno) << ")" << std::endl;
        return nullptr;
    }
    return mr2;*/
}
ibv_context *BRAMBuffer::createContext(const std::string &device_name) {
    /* There is no way to directly open the device with its name; we should get the list of devices first. */
    struct ibv_context* context = nullptr;
    int num_devices;
    struct ibv_device** device_list = ibv_get_device_list(&num_devices);
    for (int i = 0; i < num_devices; i++){
        /* match device name. open the device and return it */
        if (device_name.compare(ibv_get_device_name(device_list[i])) == 0) {
            context = ibv_open_device(device_list[i]);
            break;
        }
    }

    /* it is important to free the device list; otherwise memory will be leaked. */
    ibv_free_device_list(device_list);
    if (context == nullptr) {
        std::cerr << "BRAMBuffer:: Unable to find the device " << device_name << std::endl;
    }
    return context;
}

uint64_t BRAMBuffer::operator[](int i) const {
    return m_vptr[i];
}

uint64_t& BRAMBuffer::operator[](int i) {
    return m_vptr[i];
}

int BRAMBuffer::getFileDescriptor() const { return m_fd;}