#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9999

int main(int argc, char * argv[])
{
    int sock;
    int yes = 1;
    struct sockaddr_in broadcast_addr;
    struct sockaddr_in server_addr;
    int addr_len;
    int count;
    int ret;
    fd_set readfd;
    char buffer[1024];
    int i;

    setvbuf(stdout, NULL, _IONBF, 0); 

    printf("Inizializing...\n");

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("sock error");
        return -1;
    }

    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
    if (ret == -1)
    {
        perror("setsockopt error");
        return 0;
    }

    addr_len = sizeof(struct sockaddr_in);

    printf("Prepare broadcast message\n");

    memset((void *)&broadcast_addr, 0, addr_len);
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcast_addr.sin_port = htons(PORT);

    for (i = 0; i < 3; i++)
    {
        printf("Sending %s in broadcast\n", IP_FOUND);
        ret = sendto(sock, IP_FOUND, strlen(IP_FOUND)+1, 0, (struct sockaddr *)&broadcast_addr, addr_len);

        FD_ZERO(&readfd);
        FD_SET(sock, &readfd);

        printf("Wait message back\n");
        ret = select(sock + 1, &readfd, NULL, NULL, NULL);

        if (ret > 0)
        {
            if (FD_ISSET(sock, &readfd))
            {
                count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&server_addr, (socklen_t*) &addr_len);
                printf("Received %s\n", buffer);
                if (strstr(buffer, IP_FOUND_ACK))
                {
                    printf("Found server IP is %s, Port is %d\n", inet_ntoa(server_addr.sin_addr), htons(server_addr.sin_port));
                }
            }
        }
    }
}
