#include <rdma/rsocket.h>
#include <iostream>
#include <arpa/inet.h>

constexpr unsigned int PORT = 6969;



int main(int argc, char **argv) {
    std::cout << "Starting server..." << std::endl;
    //rdma_event_channel* rdma_channel;
    //rdma_cm_id* rdma_id,


    int sock_listener = rsocket(AF_INET, SOCK_STREAM, 0);
    if (sock_listener < 0) {
        std::cerr << "Failed to create listener socket" << std::endl;
        return -1;
    }
    std::cout << "Created listener socket" << std::endl;


    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);

    if (rbind(sock_listener, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Created socket could not be bound" << std::endl;
        rclose(sock_listener);
        return -2;
    }
    std::cout << "Successfully bound socket" << std::endl;


    if (rlisten(sock_listener, SOMAXCONN) < 0) {
        std::cerr << "Could not listen on socket" << std::endl;
        return -3;
    }
    std::cout << "Socket now listening on port " << PORT << std::endl;

    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int sock_client = -1;
    if ((sock_client = raccept(sock_listener,(sockaddr*)&client_addr, &client_addr_size)) < 0) {
        std::cerr << "Could not accept connection on socket" << std::endl;
    }
    std::cout << "Succesfully accepted connection!" << std::endl;

    std::cout << "Program teardown..." << std::endl;
    rclose(sock_client);
    rclose(sock_listener);
    std::cout << "DONE!" << std::endl;

    return 0;
}