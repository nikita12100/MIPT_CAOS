#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <wctype.h>

volatile sig_atomic_t MUST_EXIT = 0;

volatile sig_atomic_t FINISH_WOK = 0;
volatile int sock = 0;

volatile int dir_fd = 0;

unsigned const int N = 256;

enum {
    InitialCapacity = 65536,
    MaxPendingEvents = 10000
};

typedef struct { // our metadata for each event
    // file descriptor
    int fd;
    //count of reads bytes
    bool done;
} data_t;

static void
make_non_blocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK); // add O_NONBLOCK to mask
}

static data_t*
create_event_data(int epoll_fd, int fd)
{
    make_non_blocking(fd); // now made nonblock, need to put in epoll
    data_t *data = calloc(1, sizeof(*data)); // allocate memory for metadata
    data->fd = fd;

    struct epoll_event event_ready_for_read;
    event_ready_for_read.events = EPOLLIN; // processing only EPOLLIN
    event_ready_for_read.data.ptr = data; // bind metadata which we made
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_for_read); // adding to epooll

    return data;
}

static void
process_pair_ready_read(data_t *data)
{
    char recv_text[2048];
    int count;
    do {
        count = read(data->fd, &recv_text, sizeof(recv_text));
        for(int i = 0; i < count; i++) {
            recv_text[i] = towupper(recv_text[i]);
        }
        write(data->fd, &recv_text, count);

        if (0 == count) { // 0 - mean closed from other side
            data->done = true; // change flag
            close(data->fd);
        }
    } while (count > 0);
}

static void
process_epoll_event(struct epoll_event *event, size_t *files_left)
{
    const uint32_t mask = event->events;
    data_t *data = event->data.ptr;     // get metadata

    if (mask & EPOLLIN) {   // if ready to read then read
        process_pair_ready_read(data);
    }

    if(data->done) {
        *files_left -= 1; // have finish this fd and --pending
    }
}


//void read_data_and_write(size_t N, int *in)
//{
//    data_t **entries = calloc(N, sizeof(*entries)); // allocate memory for an array
//    int epoll_fd = epoll_create1(0); // create epoll, without flags
//    for (size_t i = 0; i < N; ++i) {
//        entries[i] = create_event_data(epoll_fd, in[i]); // add nonblock, metadata, and push in epoll
//    }
//
//    size_t files_left = N; // number of till non read files
//    struct epoll_event pending[MaxPendingEvents]; // queue of events
//    while (files_left > 0) {
//        int n = epoll_wait(epoll_fd, pending, MaxPendingEvents, -1);    // wait event and put them in pending
//        for (int i = 0; i < n; ++i) {       // n - count of waiting events
//            process_epoll_event(&pending[i], &files_left); // count bytes for waited filedescriptors
//        }
//    }
//
//    close(epoll_fd);
//}

void handle_sigterm_and_sigint(int signum)
{
    MUST_EXIT = 1;  // flag to exit
    if(FINISH_WOK == 1){    // flag to correctly close fd
        close(sock);
        close(dir_fd);
        exit(1);
    }
}

int main(int argc, char **argv)
{
    uint16_t port_num = strtol(argv[1], NULL, 10); // port number
    // open socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = inet_addr("127.0.0.1"), // localhost
            .sin_port = htons(port_num)
    };

    // connect server
    bind(sock, (const struct sockaddr*)&addr, sizeof(addr));

    // server listen clients
    listen(sock, SOMAXCONN);

    // processing signals
    struct sigaction action_term_and_int;
    memset(&action_term_and_int, 0, sizeof(action_term_and_int));
    action_term_and_int.sa_handler = handle_sigterm_and_sigint;     // here we change flag MUST_EXIT
    action_term_and_int.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &action_term_and_int, NULL); // listen SIGTERM


    data_t **entries = calloc(N, sizeof(*entries)); // allocate memory for an array
    int epoll_fd = epoll_create1(0); // create epoll, without flags
    make_non_blocking(sock);

    int clints_count = 0;  // number of current fd in epoll
    while (1) {
        if(MUST_EXIT == 1){
            break;  // signal
        }
        int current_connect = accept(sock, NULL, NULL); // our client
        if (current_connect != -1) {    // if there is client and no signal
            entries[clints_count] = create_event_data(epoll_fd, current_connect); // add nonblock, metadata, and push in epoll
            clints_count++;
           } 
        size_t files_left = N; // number of till non read files
        struct epoll_event pending[MaxPendingEvents]; // queue of events
        if (files_left > 0) {
            int n = epoll_wait(epoll_fd, pending, MaxPendingEvents, 100);    // wait event and put them in pending
            for (int j = 0; j < n; ++j) {       // n - count of waiting events
                process_epoll_event(&pending[j], &files_left); // count bytes for waited filedescriptors
            }
        }
    }
    // close file descriptors
    for (int j = 0; j < clints_count; ++j)
    {
        shutdown(entries[j]->fd, SHUT_RDWR);
        close(entries[j]->fd);
    }
    close(sock);

    return 0;
}