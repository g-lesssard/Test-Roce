cmake_minimum_required(VERSION 3.13)

project(test_roce)
set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(infinity/infinity)
include(rdma-core/rdma-core)
add_subdirectory(buffer_manager)


add_executable(server server.cpp)
target_link_libraries(server
        PUBLIC
        ${${PROJECT_NAME}_rdmacm}
        ${${PROJECT_NAME}_ibverbs}
        ${PROJECT_NAME}::BufferManager
)


add_executable(client client.cpp)
target_link_libraries(client
    PUBLIC
    ${${PROJECT_NAME}_rdmacm}
    ${${PROJECT_NAME}_ibverbs}
    ${PROJECT_NAME}::BufferManager
)

add_executable(ibm_client_example ibm_client_example.cpp)
target_link_libraries(ibm_client_example
    PUBLIC
    ${${PROJECT_NAME}_rdmacm}
    ${${PROJECT_NAME}_ibverbs}
    ${PROJECT_NAME}::BufferManager
)

add_executable(ibm_server_example ibm_server_example.cpp)
target_link_libraries(ibm_server_example
    PUBLIC
    ${${PROJECT_NAME}_rdmacm}
    ${${PROJECT_NAME}_ibverbs}
    ${PROJECT_NAME}::BufferManager
)

add_executable(example_infinity example_infinity.cpp)
target_link_libraries(example_infinity
        PUBLIC
        lib-infinity
)





add_executable(read_server read_server.cpp)
target_link_libraries(read_server
        PUBLIC
        ${${PROJECT_NAME}_rdmacm}
        ${${PROJECT_NAME}_ibverbs}
        ${PROJECT_NAME}::BufferManager
)

add_executable(read_client read_client.cpp)
target_link_libraries(read_client
        PUBLIC
        ${${PROJECT_NAME}_rdmacm}
        ${${PROJECT_NAME}_ibverbs}
)