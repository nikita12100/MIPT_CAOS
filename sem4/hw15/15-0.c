#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdbool.h>


enum {
    InitialCapacity = 65536,
    MaxPendingEvents = 10000
};

typedef struct { // our metadata for each event
    // file descriptor
    int fd;
    //count of reads bytes
    size_t count;
    // will be closed if we read all
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
    char buffer[4096];
    ssize_t cnt = read(data->fd, buffer, sizeof(buffer));
    if (cnt > 0) {
        data->count += cnt; // add what we have read to metadata
    }
    else if (0 == cnt) { // 0 - mean closed from other side
        data->done = true; // change flag
        close(data->fd);
    }
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


extern size_t
read_data_and_write(size_t N, int *in)
{
    data_t **entries = calloc(N, sizeof(*entries)); // allocate memory for an array 
    int epoll_fd = epoll_create1(0); // create epoll, without flags
    for (size_t i = 0; i < N; ++i) {
        entries[i] = create_event_data(epoll_fd, in[i]); // add nonblock, metadata, and push in epoll
    }

    size_t files_left = N; // number of till non read files
    struct epoll_event pending[MaxPendingEvents]; // queue of events
    while (files_left > 0) {
        int n = epoll_wait(epoll_fd, pending, MaxPendingEvents, -1);    // wait event and put them in pending
        for (int i = 0; i < n; ++i) {       // n - count of waiting events
            process_epoll_event(&pending[i], &files_left); // count bytes for waited filedescriptors
        }
    }

    close(epoll_fd);

    size_t result = 0;
    for (size_t i = 0; i < N; ++i) {    // count summ of read bytes
        result += entries[i]->count;
        free(entries[i]);
    }

    free(entries);

    return result;
}