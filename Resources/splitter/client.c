
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

#define CHUNK_SIZE 10

enum Destination {
    SOCKET,
    PIPE
};

// Client <fifo file name> <port where listen>
// Server <file> <fifo file name> <port and host where send data>

typedef struct Header {
    int index;          // Chunk index starting from 0
    int length;         // Length of the buffer
} Header;

typedef char * Payload;

// Data are sent as:
// Header + Payload

// Create and return a TCP socket to the hostname and port specified
// -1 in case of error
int create_socket(const char * hostname, int port) {

    int status;
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("cannot create socket");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, hostname, &serv_addr.sin_addr) <= 0) {
        perror("invalid address/ Address not supported");
        return -1;
    }

    printf("Connecting\n");

    if ((status = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
        perror("connection failed");
        return -1;
    }

    printf("Connected\n");

    return sock;

}

// Send a chunk of data in a TCP socket
// This call will block until the message is sent
int send_tcp_chunk(int sock, int len, char * buffer) {
    return send(sock, buffer, len, 0);
}

int send_over_socket_or_pipe(int sock_or_pipe, int index, char * chunk, int chunk_size) {
    Header header;
    header.index = index;
    header.length = chunk_size;
    int ret;

    ret = write(sock_or_pipe, &header, sizeof(Header));
    if (ret < 0) {
        perror("Cannot send bytes in socket or pipe: Header");
        return -1;
    }

    ret = write(sock_or_pipe, chunk, chunk_size);
    if (ret < 0) {
        perror("Cannot send bytes in socket or pipe: Payload");
        return -1;
    }

    return 0;
}

/**
 * A process create a named pipe and a socket to send chunk of data of a file.
 * The receiver need the be able to receive chunks and to re-create the file.
 */
int main(int argc, const char * argv[]) {

    // Server <file> <fifo file name> <port and host where send data>
    const char * file_name = argv[1];
    const char * fifo_name = argv[2];
    const char * hostname = argv[3];
    int port = atoi(argv[4]);

    int sock = create_socket(hostname, port);
    if (sock < 0) {
        perror("error creating the socket");
        return -1;
    }

    int pipe = open(fifo_name, O_WRONLY);
    if (pipe < 0) {
        perror("error creating the fifo");
        return -1;
    }

    int file_to_send = open(file_name, O_RDONLY);
    if (file_to_send < 0) {
        perror("error creating the fifo");
        return -1;
    }

    char chunk[CHUNK_SIZE];
    int index = 0;
    enum Destination destination = SOCKET;

    while (1) {
        int chunk_size = read(file_to_send, chunk, CHUNK_SIZE);
        if (chunk_size <= 0) {
            return 0;
        }
        
        switch (destination) {
            case SOCKET :
                send_over_socket_or_pipe(sock, index, chunk, chunk_size);
                destination = PIPE;
                break;
            case PIPE :
                send_over_socket_or_pipe(pipe, index, chunk, chunk_size);
                destination = SOCKET;
                break;
        }

        index ++;

    }

    return 0;

}