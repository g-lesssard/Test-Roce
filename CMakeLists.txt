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

add_executable(ibm_client_example ibm_client_example.cpp)
target_link_libraries(ibm_client_example
        PUBLIC
        rdmacm
        ibverbs
        )

add_executable(ibm_server_example ibm_server_example.cpp)
target_link_libraries(ibm_server_example
        PUBLIC
        rdmacm
        ibverbs
        )