#include <rdma/rsocket.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

constexpr unsigned int PORT = 6968;
constexpr const uint64_t expected_message[] = {0xDEADBEEFFACEB00C,
                                      0x6969696969696969,
                                      0x420};

char message[100];
size_t file_size = 0x2000;
off_t base_addr = 0xA0000000;
char file_name[] = "/dev/mem";

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
    int fd = open(file_name, O_RDWR | O_SYNC);
    if (fd == -1) {
        std::cerr << "Failed to create/open file buffer" << std::endl;
        return -5;
    }
    //write(fd, "test", sizeof("test"));
    //read(fd, message, sizeof("test\n"));
    //std::cout << "Starting message is: " << message << std::endl;
    auto * buffer_vptr = (uint64_t *) mmap(NULL, file_size, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, fd, base_addr);
    if (buffer_vptr == nullptr) {
        close(fd);
        std::cerr << "Failed to get virtual pointer to file" << std::endl;
        return -6;
    }
    buffer_vptr[0] = 0xDEADBEEFFACEB00C;
    std::cout << "Starting message is: " << std::hex << buffer_vptr[1] << std::endl;
    std::cout << "Successfully obtained virtual pointer" << std::endl;

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
        return -4;
    }

    std::cout << "Successfully accepted connection!" << std::endl;
    ssize_t  size = 0;
    for (int i = 0; i < 3; i++ ) {
        if (rrecv(sock_client, (uint64_t*)&buffer_vptr[i], 8, MSG_WAITALL) < 0)
            std::cerr << "[ERROR] Failed to receive word";
    }
    //size = rrecv(sock_client, buffer_vptr, sizeof(expected_message), MSG_WAITALL);
    std::cout << "[INFO] Received message(" << size << "): " << buffer_vptr << std::endl;
    std::cout << "Program teardown..." << std::endl;
    rclose(sock_client);
    rclose(sock_listener);
    close(fd);
    std::cout << "DONE!" << std::endl;

    return 0;
}