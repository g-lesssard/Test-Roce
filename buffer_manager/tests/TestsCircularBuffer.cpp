#include <gtest/gtest.h>
#include "buffer_manager/CircularBuffer.h"
#include "buffer_manager/StaticBuffer.h"

constexpr uint16_t BUFFER_SIZE = 32;

class TestsCircularBuffer : public testing::Test {
protected:

    StaticBuffer<uint8_t, BUFFER_SIZE> m_bytesBuffer = {};
    StaticBuffer<uint32_t, BUFFER_SIZE> m_wordsBuffer = {};

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


TEST_F(TestsCircularBuffer, CircularBuff_getFreeSize_Empty) {
    // Given
    // Then
    uint16_t retBytes = m_bytesCircularBuffer->getFreeSize();
    uint16_t retWords = m_wordsCircularBuffer->getFreeSize();

    // Expect
    EXPECT_EQ(retBytes, 32);
    EXPECT_EQ(retWords, 32);
}


TEST_F(TestsCircularBuffer, CircularBuff_getDataLength_Empty) {
    // Given
    // Then
    uint16_t retBytes = m_bytesCircularBuffer->getDataLength();
    uint16_t retWords = m_wordsCircularBuffer->getDataLength();

    // Expect
    EXPECT_EQ(retBytes, 0);
    EXPECT_EQ(retWords, 0);
}

TEST_F(TestsCircularBuffer, CircularBuff_getFreeSize_AfterWrite) {
    // Given
    uint8_t dataBytes[2];
    uint32_t dataWords[2];

    // Then
    ASSERT_TRUE(m_bytesCircularBuffer->put(dataBytes, 2));
    ASSERT_TRUE(m_wordsCircularBuffer->put(dataWords, 2));
    uint16_t retBytes = m_bytesCircularBuffer->getFreeSize();
    uint16_t retWords = m_wordsCircularBuffer->getFreeSize();

    // Expect
    EXPECT_EQ(retBytes, BUFFER_SIZE-2);
    EXPECT_EQ(retWords, BUFFER_SIZE-2);
}

TEST_F(TestsCircularBuffer, CircularBuff_getDataLength_AftrWrite) {
    // Given
    uint8_t dataBytes[2];
    uint32_t dataWords[2];

    // Then
    ASSERT_TRUE(m_bytesCircularBuffer->put(dataBytes, 2));
    ASSERT_TRUE(m_wordsCircularBuffer->put(dataWords, 2));
    uint16_t retBytes = m_bytesCircularBuffer->getDataLength();
    uint16_t retWords = m_wordsCircularBuffer->getDataLength();

    // Expect
    EXPECT_EQ(retBytes, 2);
    EXPECT_EQ(retWords, 2);
}

TEST_F(TestsCircularBuffer, CircularBuff_getFreeSize_AfterWriteFull) {
    // Given
    uint8_t dataBytes[BUFFER_SIZE];
    uint32_t dataWords[BUFFER_SIZE];

    // Then
    ASSERT_TRUE(m_bytesCircularBuffer->put(dataBytes, BUFFER_SIZE));
    ASSERT_TRUE(m_wordsCircularBuffer->put(dataWords, BUFFER_SIZE));
    uint16_t retBytes = m_bytesCircularBuffer->getFreeSize();
    uint16_t retWords = m_wordsCircularBuffer->getFreeSize();

    // Expect
    EXPECT_EQ(retBytes, 0);
    EXPECT_EQ(retWords, 0);
    EXPECT_TRUE(m_bytesCircularBuffer->isFull());
    EXPECT_TRUE(m_wordsCircularBuffer->isFull());
}

TEST_F(TestsCircularBuffer, CircularBuff_getDataLength_AftrWriteFull) {
    // Given
    uint8_t dataBytes[BUFFER_SIZE];
    uint32_t dataWords[BUFFER_SIZE];

    // Then
    ASSERT_TRUE(m_bytesCircularBuffer->put(dataBytes, BUFFER_SIZE));
    ASSERT_TRUE(m_wordsCircularBuffer->put(dataWords, BUFFER_SIZE));
    uint16_t retBytes = m_bytesCircularBuffer->getDataLength();
    uint16_t retWords = m_wordsCircularBuffer->getDataLength();

    // Expect
    EXPECT_EQ(retBytes, BUFFER_SIZE);
    EXPECT_EQ(retWords, BUFFER_SIZE);
}

TEST_F(TestsCircularBuffer, CircularBuff_Put_then_Get_NoWrap) {
    // Given
    uint8_t dataBytes[6] = {0,1,2,3,4,5};
    uint32_t dataWords[6] = {0,1,2,3,4,5};
    uint8_t dataBytesRead[6];
    uint32_t dataWordsRead[6];

    // Then
    ASSERT_TRUE(m_bytesCircularBuffer->put(dataBytes, 6));
    ASSERT_TRUE(m_wordsCircularBuffer->put(dataWords, 6));
    ASSERT_EQ(m_bytesCircularBuffer->get(dataBytesRead, 6),6);
    ASSERT_EQ(m_wordsCircularBuffer->get(dataWordsRead, 6),6);

    // Expect
    for (int i =0; i< 6; i++) {
        EXPECT_EQ(dataBytes[i], dataBytesRead[i]);
        EXPECT_EQ(dataWords[i], dataWordsRead[i]);
    }
}


TEST_F(TestsCircularBuffer, CircularBuff_Put_then_Get_WithWrap) {
    // Given
    uint8_t dataBytes[20];
    uint32_t dataWords[20];
    for (int i = 0; i < 20; i++) {
        dataBytes[i] = i;
        dataWords[i] = i;
    }
    uint8_t dataBytesRead[20];
    uint32_t dataWordsRead[20];

    // Then
    ASSERT_TRUE(m_bytesCircularBuffer->put(dataBytes, 20));
    ASSERT_TRUE(m_wordsCircularBuffer->put(dataWords, 20));
    ASSERT_EQ(m_bytesCircularBuffer->get(dataBytesRead, 20),20);
    ASSERT_EQ(m_wordsCircularBuffer->get(dataWordsRead, 20),20);

    // Expect
    for (int i =0; i< 6; i++) {
        EXPECT_EQ(dataBytes[i], dataBytesRead[i]);
        EXPECT_EQ(dataWords[i], dataWordsRead[i]);
    }

    // Then
    for (int i = 0; i < 20; i++) {
        dataBytes[i] += 15;
        dataWords[i] += 15;
    }

    ASSERT_TRUE(m_bytesCircularBuffer->put(dataBytes, 20));
    ASSERT_TRUE(m_wordsCircularBuffer->put(dataWords, 20));
    ASSERT_EQ(m_bytesCircularBuffer->get(dataBytesRead, 20),20);
    ASSERT_EQ(m_wordsCircularBuffer->get(dataWordsRead, 20),20);

    // Expect
    for (int i =0; i< 6; i++) {
        EXPECT_EQ(dataBytes[i], dataBytesRead[i]);
        EXPECT_EQ(dataWords[i], dataWordsRead[i]);
    }
}