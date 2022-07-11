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
    inet_pton(AF_INET, DEST_ADDR, &server_addr.sin_addr);

    if (rconnect(socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection could not be established!\n";
        return -6;
    }
    std::cout << "[INFO] Connection established.\n";
    char message[] = "Hello RDMA!";
    if (rsend(socket, message, sizeof(message), 0) < 0)
        std::cerr << "[ERROR] Failed to send message";
    std::cout << "Program teardown..." << std::endl;
    rclose(socket);
    std::cout << "DONE!" << std::endl;


}