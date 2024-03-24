#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <ifaddrs.h>

#define DEBUG

#define FIRST_SEQUENCE 0

int last_sequence = FIRST_SEQUENCE;

#define PORT 9999

typedef struct
{
    int id;
    int socket;
} Node;

typedef struct Message
{
    int from;
    int random_value;
    int sequence;
} Message;

Message *alloc_msg(int from, int random_value)
{
    Message *msg = malloc(sizeof(Message));
    msg->from = from;
    msg->random_value = random_value;
    msg->sequence = last_sequence;
    return msg;
}

void handler_sigchild(int signal_no)
{
    int child_status = -1;
    wait(&child_status);
}

void exit_with_error(char *string)
{
    perror(string);
    exit(1);
}

int bcast(Node *node, int random_value, int sequence)
{
    Message *msg = alloc_msg(node->id, random_value);

    msg->sequence = sequence;

    int addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in broadcast_addr;
    memset((void *)&broadcast_addr, 0, addr_len);
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcast_addr.sin_port = htons(PORT);

    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);
    tmp = addrs;
    while (tmp)
    {
        int ret = setsockopt(node->socket, SOL_SOCKET, SO_BINDTODEVICE, tmp->ifa_name, sizeof(tmp->ifa_name));
        if (ret < 0)
        {
            exit_with_error("recv socket bind interface");
        }

        ret = sendto(node->socket, msg, sizeof(Message), 0, (struct sockaddr *)&broadcast_addr, addr_len);
        if (ret < 0)
        {
            exit_with_error("send to in bcast");
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
    free(msg);

    // // Listen from all interfaces
    // int ret = setsockopt(node->socket, SOL_SOCKET, SO_BINDTODEVICE, NULL, NULL);
    // if (ret < 0)
    // {
    //     exit_with_error("recv socket bind interface");
    // }

    return 0;
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int process_leader(Node *node)
{
#ifdef DEBUG
    printf("%d: inizializing Leader...\n", node->id);
#endif

    msleep(3000);

    int random_value = rand() % 100;

    printf("%d: BCAST started: %d\n", node->id, random_value);
    int res = bcast(node, random_value, 0);
    if (res < 0)
    {
        perror("error sending broadcast");
        return 1;
    }

    return 0;
}

int process_node(Node *node)
{
    socklen_t addr_len;
    struct sockaddr_in client_addr;
    fd_set readfd;
    Message msg;

#ifdef DEBUG
    printf("%d: inizializing Node...\n", node->id);
#endif

    int espected_count = 0;

    while (1)
    {
        FD_ZERO(&readfd);
        FD_SET(node->socket, &readfd);

        struct timeval tv = {1, 0};
        int ret = select(node->socket + 1, &readfd, NULL, NULL, &tv);
        if (ret > 0)
        {
            if (FD_ISSET(node->socket, &readfd))
            {
                // printf("%d start receiving\n", node->id);
                int count = recvfrom(node->socket, &msg, sizeof(Message), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);

                // printf("%d: msg->sequence %d, count: %d\n", node->id, msg.sequence, espected_count);
                if (msg.sequence >= espected_count)
                {
                    espected_count++;

                    printf("%d: recv %d from %d (%s): %d (bytes: %d)\n", node->id, msg.random_value, msg.from, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), count);

                    int s = rand() % 100 + 10;
                    int random_value = msg.random_value;
                    int sequence = msg.sequence;

                    msleep(s);
                    int ret = bcast(node, random_value, sequence);
                    if (ret < 0)
                    {
                        exit_with_error("Error sending broadcast");
                    }
                }
            }
        }
    }
}

int create_bcast_socket()
{
    int ret;
    int yes = 1;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        exit_with_error("recv socket");
    }

    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
    if (ret == -1)
    {
        perror("setsockopt error");
        return 0;
    }

    struct sockaddr_in server_addr;
    int addr_len = sizeof(struct sockaddr_in);
    memset((void *)&server_addr, 0, addr_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    ret = bind(sock, (struct sockaddr *)&server_addr, addr_len);
    if (ret < 0)
    {
        exit_with_error("recv socket bind");
    }

    return sock;
}

Node *create_node(int id)
{
    Node *node = malloc(sizeof(Node));
    node->id = id;
    node->socket = create_bcast_socket();
    return node;
}

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc < 2)
    {
        exit_with_error("No neighbor for this node");
    }

    int id = atoi(argv[1]);

    Node *node = create_node(id);

    srand(id);

    // Leader
    if (id == 0)
    {
        return process_leader(node);
    }
    else
    {
        return process_node(node);
    }
}