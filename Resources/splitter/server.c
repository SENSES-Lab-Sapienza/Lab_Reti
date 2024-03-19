#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/select.h>

#define CHUNK_SIZE 10

typedef struct Header {
    int index;          // Chunk index starting from 0
    int length;         // Length of the buffer
} Header;

typedef char * Payload;

// Create and return a TCP socket to the hostname and port specified
// -1 in case of error
int create_server_socket_and_accept_a_client(int port) {

    int sock, client_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return -1;
    }

    // Forcefully attaching socket to the port
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt error");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    printf("Binding\n");

    // Forcefully attaching socket to the port 8080
    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }

    printf("Listening\n");

    if (listen(sock, 3) < 0) {
        perror("listen");
        return -1;
    }

    printf("Accepting\n");

    if ((client_socket = accept(sock, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("cannot accept new socket");
        return -1;
    }

    printf("Connected\n");

    return client_socket;

}

int main(int argc, const char * argv[]) {

    // Client <fifo file name> <port where listen>
    const char * fifo_name = argv[1];
    int port = atoi(argv[2]);

    // Open client for data
    int client_sock = create_server_socket_and_accept_a_client(port);

    int pipe = open(fifo_name, O_RDONLY);
    if (pipe < 0) {
        perror("error creating the fifo");
        return -1;
    }

    fd_set readfd;

    while (1) {
        FD_ZERO(&readfd);
        FD_SET(pipe, &readfd);
        FD_SET(client_sock, &readfd);

        // printf("Fifo: %d, Socket: %d", pipe, client_sock);

        // printf("Waiting data\n");

        int ret = select(client_sock + 1, &readfd, NULL, NULL, 0);

        // printf("Select ret: %d\n", ret);

        if (ret > 0) {

            if (FD_ISSET(client_sock, &readfd)) {
                // read from socket
                printf("read from socket\n");
                continue;
            }

            if (FD_ISSET(pipe, &readfd)) {
                // read from pipe
                printf("read from pipe\n");
                continue;
            }
        
        }

    }

    return 0;

}