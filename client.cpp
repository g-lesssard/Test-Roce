#include <rdma/rsocket.h>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>

constexpr unsigned int DEST_PORT = 6969;
constexpr unsigned int SOURCE_PORT = 4242;
constexpr const char DEST_ADDR[] = "10.0.0.69";

int main(int argc, char **argv) {
    int socket = rsocket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        std::cerr << "Failed to create client socket" << std::endl;
        return -1;
    }
    std::cout << "Created client socket" << std::endl;

    sockaddr_in server_addr;
    hostent *server;
    server = gethostbyname(DEST_ADDR);
    if (!server) {
        std::cerr << "No such destination!\n";
        return -3;
    }
    std::cout << "Destination is found.\n";

    std::memset((char*)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEST_PORT);

    // Check if there is an address of the host
    if (server->h_addr_list[0])
        std::memcpy((char*)server->h_addr_list[0], (char*)&server_addr.sin_addr, server->h_length);
    else {
        std::cerr << "There is no a valid address for that hostname!\n";
        return -5;
    }

    if (connect(socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection could not be established!\n";
        return -6;
    }
    std::cout << "[INFO] Connection established.\n";
    std::cout << "Program teardown..." << std::endl;
    rclose(socket);
    std::cout << "DONE!" << std::endl;


}