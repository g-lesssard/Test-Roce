set(TEST_NAME test-buffer-manager)

set(TEST_SOURCES
    TestsCircularBuffer.cpp
)

set(TEST_HEADERS

)

add_executable(${TEST_NAME} ${TEST_SOURCES} ${TEST_HEADERS})

target_link_libraries(${TEST_NAME}
        PUBLIC
        gtest
        gtest_main
        gmock
        ${PROJECT_NAME}::BufferManager
        )

add_test(NAME ${TEST_NAME}
        COMMAND ${TEST_NAME})