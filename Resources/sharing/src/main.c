#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <ifaddrs.h>
#include <sys/socket.h>

typedef struct Message {
    int source;
    int sequence;
    int payload_lenght;
    char * payload;
} Message;

typedef struct Payload {
    int value;
} Payload;

Message * prepare_message(Payload * payload);

//---------------
// Traffic Generator
//---------------

// Some sistem that triggers handler callback every x milliseconds

//---------------
// Broadcaster
//---------------

typedef void (*BroadcasterHandler)(void * broadcaster, Payload * payload);

typedef struct Broadcaster {
    // .. Local data to manage broadcast
    // e.g. socket opened, network interfaces, 
    // sequence of other nodes...

    BroadcasterHandler handler;
} Broadcaster;

void register_handler(Broadcaster * broadcaster, BroadcasterHandler handler) {
    broadcaster->handler = handler;
}

void send(Broadcaster * broadcaster, Payload * payload) {
    // Send UDP packet and update local sequence
}

void process_broadcaster(Broadcaster * broadcaster) {
    // In case of packet reception, notify the handler
    // Discard already seen packets
}

//---------------
// Traffic Analyzer
//---------------

typedef struct TrafficAnalyzer {

    // Definition of sliding window...

} TrafficAnalyzer;

void received_pkt(TrafficAnalyzer * analyzer, int source) {
    // Record the packet send and datetime of it
}

void dump(TrafficAnalyzer * analyzer) {
    // Dump information about the thoughput of all packets received
}

//-------------------------
// Utility
// ------------------------

/**
 * Bind the given socket to all interfaces (one by one)
 * and invoke the handler with same parameter
 */
void bind_to_all_interfaces(int sock, void * context, void (*handler)(int, void *)) {
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);
    tmp = addrs;
    while (tmp){
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
            setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, tmp->ifa_name, sizeof(tmp->ifa_name));
            handler(sock, context);
        }
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
}

/**
 * Sleep a given amount of milliseconds
 */
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


int main() {

    // Autoflush stdout for docker
    setvbuf(stdout, NULL, _IONBF, 0);

    // Traffic generator

    // Broadcaster

    // Traffic analyzer

}