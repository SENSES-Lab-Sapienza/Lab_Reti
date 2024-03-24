#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void exit_with_error(const char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

// https://redis.io/docs/reference/protocol-spec/

int main(int argc, const char * argv[]) {

    // Open Socket and receive connections

    // Keep a key, value store (you are free to use any data structure you want)

    // Create a process for each connection to serve set and get requested

}

