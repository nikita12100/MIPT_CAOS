#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

volatile sig_atomic_t was_interrupt = 0;
int my_errno = 0;

void sig_handler(int signum)
{
    was_interrupt = 1;
    return;
}

int write_fully(int fd, const char* buffer, size_t len)
{
    int bytes_total_written = 0;
    int bytes_written = 0;
    while (bytes_total_written != len) {
        bytes_written = write(fd, 
                            buffer + bytes_total_written, 
                            len - bytes_total_written);
        if (bytes_written <= 0) {
            return -1;
        } else {
            bytes_total_written += bytes_written;
        }
    }
    return len;
}

int process_data_block(int client_socket) {
    int bytes = 0;
    int bytes_processed = 0;
    char buffer[4096];
    do  {

        bytes = read(client_socket, buffer, sizeof(buffer));
        if (bytes == 0) {  //EOF
            close(client_socket);
        
        } else if (bytes > 0) {  // OK
            for (int i = 0; i < bytes; ++i) {
                if (islower(buffer[i])) {
                    buffer[i] = toupper(buffer[i]);
                }
            }
            write_fully(client_socket, buffer, bytes);
            bytes_processed += bytes;
        
        } else if (errno != EAGAIN) { // error
            perror("read");
            return -1;
        }
    } while (bytes > 0);

    return bytes_processed;
}


int main(int argc, char** argv)
{
    // signal handler setting
    struct sigaction sig_action;
    memset(&sig_action, 0, sizeof(sig_action));
    sig_action.sa_handler = sig_handler;
    
    sigaction(SIGTERM, &sig_action, NULL);

    // epoll creating
    int epoll = epoll_create(1);
    
    // server configuration
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int16_t port = strtol(argv[1], NULL, 10);
    
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };
    my_errno = bind(server_socket, 
        (const struct sockaddr*)&server_addr, 
        sizeof(server_addr));
    if (my_errno < 0) {
        return 1;
        perror("bind");
    }

    listen(server_socket, SOMAXCONN);

    // setting nonblocking flag to server socket
    int sock_flags = fcntl(server_socket, F_GETFL);
    fcntl(server_socket, F_SETFL, sock_flags | O_NONBLOCK);
    
    // processing clients
    int* client_fds = calloc(256, sizeof(int));
    int client_fds_count = 256;
    int curr_client = 0;
    struct epoll_event* ready_fds = calloc(256, sizeof(struct epoll_event));

    while (!was_interrupt) {
        // accepting
        int client_socket = accept(server_socket, NULL, NULL);
 
        if (client_socket >= 0) {
            // setting nonblocking I/O
            int client_flags = fcntl(client_socket, F_GETFL);
            fcntl(client_socket, F_SETFL, client_flags | O_NONBLOCK);

            // adding to epoll
            struct epoll_event ready_to_read = {
                .events = EPOLLIN,
                .data.fd = client_socket
            };
            epoll_ctl(epoll, EPOLL_CTL_ADD, client_socket, &ready_to_read);

            // storing client's fd
            client_fds[curr_client] = client_socket;
            curr_client++;
        } else if (errno != EAGAIN) {
            perror("accept");
        }

        // processing events
        int ev_count = epoll_wait(epoll, ready_fds, 256, 100);
        if (ev_count < 0) {
            perror("epoll_wait");
        
        } else if (ev_count > 0) {  // events detected
            for (int i = 0; i < ev_count; ++i) {
                process_data_block(ready_fds[i].data.fd);
            }
        }
    }

    for (int i = 0; i < curr_client; ++i) {
        shutdown(client_fds[i], SHUT_RDWR);
        close(client_fds[i]);
    }

    free(client_fds);
    free(ready_fds);
    close(server_socket);
    return 0;
}