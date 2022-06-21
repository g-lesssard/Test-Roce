project(test_roce)

set(CMAKE_CXX_STANDARD 20)


add_executable(server server.cpp)
target_link_libraries(server
        PUBLIC
        rdmacm
        ibverbs
)


add_executable(client client.cpp)
target_link_libraries(client
    PUBLIC
        rdmacm
        ibverbs
)