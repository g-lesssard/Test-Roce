#include <rdma/rsocket.h>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>

constexpr unsigned int DEST_PORT = 6968;
constexpr const char DEST_ADDR[] = "10.42.0.105";

constexpr const uint64_t message[] = {0xDEADBEEFFACEB00C,
                                      0x6969696969696969,
                                      0x420};

#define NO_RDMA
#ifdef NO_RDMA
#define rsocket socket
#define raccept accept
#define rbind bind
#define rlisten listen
#define raccept accept
#define rconnect connect
#define rsend send
#define rrecv recv
#define rclose close
#endif

int main(int argc, char **argv) {
    int sock = rsocket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[ERROR] Failed to create client socket" << std::endl;
        return -1;
    }
    std::cout << "[INFO] Created client socket" << std::endl;

    sockaddr_in server_addr;
    hostent *server;
    server = gethostbyname(DEST_ADDR);
    if (!server) {
        std::cerr << "[ERROR] No such destination!\n";
        return -3;
    }
    std::cout << "[INFO] Destination is found.\n";

    std::memset((char*)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEST_PORT);
    inet_pton(AF_INET, DEST_ADDR, &server_addr.sin_addr);

    if (rconnect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[ERROR] Connection could not be established!\n";
        return -6;
    }
    std::cout << "[INFO] Connection established.\n";
    for (int i = 0; i < 3; i++ ) {
        if (rsend(sock, (uint64_t*)&message[i], 8, 0) < 0)
            std::cerr << "[ERROR] Failed to send message";
    	std::cout << "[INFO] Bytes sent: " << std::hex << message[i] << std::endl;
    }
    std::cout << "[INFO] Program teardown..." << std::endl;
    rclose(sock);
    std::cout << "[INFO] DONE!" << std::endl;


}
