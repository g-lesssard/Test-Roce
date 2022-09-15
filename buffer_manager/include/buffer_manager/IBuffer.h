#ifndef TEST_ROCE_IBUFFER_H
#define TEST_ROCE_IBUFFER_H

#include <cstdint>
#include <infiniband/verbs.h>

template<class type>
class IBuffer {
public:
    virtual ~IBuffer() = default;

    virtual type* getAddress() const = 0;
    virtual std::size_t getMaxSize() const = 0;
    virtual std::size_t getAlignment() const = 0;
    virtual void clear() = 0;
    virtual ibv_mr* registerBuffer(ibv_pd* pd, unsigned int access) = 0;

    virtual type operator [](uint32_t i) const = 0;
    virtual type& operator [](uint32_t i) = 0;

};

#endif //TEST_ROCE_IBUFFER_H
