#include <gtest/gtest.h>
#include "buffer_manager/CircularBuffer.h"
#include "buffer_manager/StaticBuffer.h"

class TestsCircularBuffer : public testing::Test {
protected:
    CircularBufferResources<uint8_t> m_bytesCircularBufferResources;
    CircularBufferResources<uint32_t> m_wordsCircularBufferResources;
    StaticBuffer<uint8_t, 32> m_bytesBuffer;
    StaticBuffer<uint32_t, 32> m_wordsBuffer;

    CircularBuffer<uint8_t>* m_bytesCircularBuffer;
    CircularBuffer<uint32_t>* m_wordsCircularBuffer;

    void SetUp() override {
        m_bytesCircularBuffer = new CircularBuffer<uint8_t>(m_bytesBuffer);
        m_wordsCircularBuffer = new CircularBuffer<uint32_t>(m_wordsBuffer);
    }

    void TearDown() override {
        delete m_bytesCircularBuffer;
        delete m_wordsCircularBuffer;
    }

};


TEST_F(TestsCircularBuffer, Dummy) {
    ASSERT_TRUE(true);
    ASSERT_FALSE(m_bytesCircularBuffer->isFull());
}