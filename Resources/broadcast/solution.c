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

// #define DEBUG

#define FIRST_SEQUENCE 0

int last_sequence = FIRST_SEQUENCE;

#define PORT 9999

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

int is_neighbor(int id, int from, int num_neighbors, int *neighbors)
{
    for (int i = 0; i < num_neighbors; i++)
    {
        if (neighbors[i] == from)
        {
            // printf("%d: %d is my neighbor\n", id, from);
            return 1;
        }
    }
    return 0;
}

int bcast(int sock, int id, int random_value, int sequence)
{
    int addr_len = sizeof(struct sockaddr_in);
    fd_set writefd;
    int ret;
    int yes = 1;
    char *addr;

    Message *msg = alloc_msg(id, random_value);
    msg->sequence = sequence;

    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
    if (ret == -1)
    {
        perror("setsockopt error");
        return 0;
    }

    struct sockaddr_in broadcast_addr;
    memset((void *)&broadcast_addr, 0, addr_len);
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    broadcast_addr.sin_port = htons(PORT);

    struct ifaddrs *addrs, *tmp;

    getifaddrs(&addrs);
    tmp = addrs;
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
            setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, tmp->ifa_name, sizeof(tmp->ifa_name));
            sendto(sock, msg, sizeof(Message), 0, (struct sockaddr *)&broadcast_addr, addr_len);
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
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

int leader(int sock, int id, int num_neighbors, int *neighbors)
{
#ifdef DEBUG
    printf("%d: inizializing...\n", id);
#endif

    msleep(3000);

    int random_value = rand() % 100;

    printf("%d: BCAST started: %d\n", id, random_value);
    int res = bcast(sock, id, random_value, id);
    if (res < 0)
    {
        perror("error sending broadcast");
        return 1;
    }

    return 0;
}

int node(int sock, int id, int num_neighbors, int *neighbors)
{
    int yes = 1;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t addr_len;
    int count;
    int ret;
    fd_set readfd;
    Message msg;
    int espected_count = 0;

#ifdef DEBUG
    printf("%d: inizializing...\n", id);
#endif

    struct timeval stop, start;
    gettimeofday(&start, NULL);

#ifdef DEBUG
    printf("%d: started\n", id);
#endif

    while (1)
    {
        gettimeofday(&stop, NULL);
        if ((stop.tv_sec - start.tv_sec) > 1115)
        {
            printf("%d: timeout: exit\n", id);
            return 0;
        }

        FD_ZERO(&readfd);
        FD_SET(sock, &readfd);

        struct timeval tv = {1, 0};
        ret = select(sock + 1, &readfd, NULL, NULL, &tv);
        if (ret > 0)
        {
            if (FD_ISSET(sock, &readfd))
            {
                // printf("%d start receiving\n", id);
                count = recvfrom(sock, &msg, sizeof(Message), 0, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len);

                if (!is_neighbor(id, msg.from, num_neighbors, neighbors))
                {
                    continue;
                }

                // printf("%d: msg->sequence %d, count: %d\n", id, msg.sequence, espected_count);
                if (msg.sequence >= espected_count)
                {
                    espected_count++;

                    printf("%d: recv %d from %d (%s): %d (bytes: %d)\n", id, msg.random_value, msg.from, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), count);

                    int s = rand() % 100 + 10;
                    int random_value = msg.random_value;
                    int sequence = msg.sequence;

                    msleep(s);
                    int ret = bcast(sock, id, random_value, sequence);
                    if (ret < 0)
                    {
                        perror("Error sending broadcast");
                    }

                    //                     pid_t child_pid = fork();
                    //                     switch (child_pid)
                    //                     {
                    //                     case -1:
                    //                     {
                    //                         perror("Child not created");
                    //                         return 1;
                    //                     }
                    //                     case 0:
                    //                     {
                    // #ifdef DEBUG
                    //                         printf("%d sleep %d\n", id, s);
                    // #endif
                    //                         msleep(s);

                    // #ifdef DEBUG
                    //                         printf("%d: BCAST starting: %d\n", id, random_value);
                    // #endif

                    //                         int ret = bcast(sock, id, random_value, sequence);

                    // #ifdef DEBUG
                    //                         printf("%d: BCAST propagated: %d, result: %d\n", id, random_value, ret);
                    // #endif

                    //                         if (ret < 0)
                    //                         {
                    //                             perror("Error sending broadcast");
                    //                         }
                    //                         return 0;
                    //                     }
                    //                     default:
                    //                         break;
                    //                     }
                }
            }
        }
    }
}

void handler_sigchild(int signal_no)
{
    int child_status = -1;
    wait(&child_status);
}

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    signal(SIGCHLD, handler_sigchild);

    if (argc < 3)
    {
        perror("No neighbor for this node");
        return 1;
    }

    int id = atoi(argv[1]);

    int *neighbors = calloc(sizeof(int), argc - 2);
    for (int i = 0; i < argc - 2; i++)
    {
        neighbors[i] = atoi(argv[i + 2]);
        // printf("%d: neighbor: %d\n", id, neighbors[i]);
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("sock error\n");
        return -1;
    }

    socklen_t addr_len = (socklen_t)sizeof(struct sockaddr_in);

    struct sockaddr_in server_addr;
    memset((void *)&server_addr, 0, addr_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    int ret = bind(sock, (struct sockaddr *)&server_addr, (socklen_t)addr_len);
    if (ret < 0)
    {
        perror("bind error\n");
        return -1;
    }

    srand(id);

    // Leader
    if (id == 0)
    {
        return leader(sock, id, argc - 2, neighbors);
    }

    // Node
    return node(sock, id, argc - 2, neighbors);
}