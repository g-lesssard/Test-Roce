#ifndef TEST_ROCE_STATICBUFFER_H
#define TEST_ROCE_STATICBUFFER_H

#include "buffer_manager/IBuffer.h"
#include <array>

template<class type, std::size_t size>
class StaticBuffer : public IBuffer {
public:
    StaticBuffer();
    ~StaticBuffer() override = default;

    void* getAddress() const override;
    std::size_t getMaxSize() const override;
    std::size_t getAlignment() const override;
    void clear() override;
    ibv_mr * registerBuffer(ibv_pd* pd, unsigned int access) override;

    std::array<type, size>& getContainer();
    type operator [](uint i) const;
    type& operator [](uint i);

private:
    std::array<type, size> m_array;
};

#include "StaticBuffer.tpp"

#endif //TEST_ROCE_STATICBUFFER_H
