#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9999

int main(int argc, char *argv[])
{
    int sock;
    int yes = 1;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    int addr_len;
    int count;
    int ret;
    fd_set readfd;
    char buffer[1024];

    setvbuf(stdout, NULL, _IONBF, 0); 

    printf("Inizializing...\n");

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("sock error\n");
        return -1;
    }

    addr_len = sizeof(struct sockaddr_in);

    memset((void *)&server_addr, 0, addr_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    ret = bind(sock, (struct sockaddr *)&server_addr, addr_len);
    if (ret < 0)
    {
        perror("bind error\n");
        return -1;
    }
    while (1)
    {
        printf("Server started\n");
        FD_ZERO(&readfd);
        FD_SET(sock, &readfd);

        printf("Waiting connections\n");

        ret = select(sock + 1, &readfd, NULL, NULL, 0);
        if (ret > 0)
        {
            if (FD_ISSET(sock, &readfd))
            {
                count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&client_addr, (socklen_t*) &addr_len);
                printf("Message received: %s\n", buffer);
                if (strstr(buffer, IP_FOUND))
                {
                    printf("Client connection information:\n\t IP: %s, Port: %d\n",
                           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                    memcpy(buffer, IP_FOUND_ACK, strlen(IP_FOUND_ACK) + 1);
                    printf("Sending %s message back to client\n", IP_FOUND_ACK);
                    count = sendto(sock, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_addr, addr_len);
                }
            }
        }
    }
}