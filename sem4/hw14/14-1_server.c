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

volatile sig_atomic_t MUST_EXIT = 0;
volatile sig_atomic_t FINISH_WOK = 0;

volatile int sock = 0;
volatile int dir_fd = 0;

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
    uint16_t port_num = strtol(argv[1], NULL, 10);
    // open socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        perror("socket");
        abort();
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = inet_addr("127.0.0.1"), // localhost
            .sin_port = htons(port_num)
    };

    // connect server
    int my_connect = bind(sock, (const struct sockaddr*)&addr, sizeof(addr));
    if (my_connect) {
        perror("bind");
        close(sock);
        return 1;
    }

    // server listen clients
    int my_listen = listen(sock, SOMAXCONN);
    if (my_listen) {
        perror("listen");
        close(sock);
        return 1;
    }

    // processing signals
    struct sigaction action_term_and_int;
    memset(&action_term_and_int, 0, sizeof(action_term_and_int));
    action_term_and_int.sa_handler = handle_sigterm_and_sigint;     // here we change flag MUST_EXIT
    action_term_and_int.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &action_term_and_int, NULL); // listen SIGTERM
    sigaction(SIGINT, &action_term_and_int, NULL);  //  listen SIGINT


    dir_fd = open(argv[2], O_DIRECTORY);    // to not open everytime

    while (1) {
        if(MUST_EXIT == 1){
            break;  // signal
        }
        FINISH_WOK = 1;
        int current_connect = accept(sock, NULL, NULL); // our client
        if (current_connect != -1) {    // if there is client and no signal
            FINISH_WOK = 0;
            FILE* file = fdopen(current_connect, "r");
            char file_name[4096];
            fscanf(file, "GET %s HTTP/1.1", file_name);

            // check is file exists
            if (-1 == faccessat(dir_fd, file_name, F_OK, 0)) {       //file do not exist
                char answer_file_size[256];
                strcat(answer_file_size, "HTTP/1.1 404 Not Found\r\n"
                                         "Content-Length: 0\r\n\r\n");

                write(current_connect, answer_file_size, strlen(answer_file_size));

                close(current_connect);
                continue;
            }

            // check is file readable
            if (-1 == faccessat(dir_fd, file_name, R_OK, 0)) {
                char answer_file_size[256];

                strcat(answer_file_size, "HTTP/1.1 403 Forbidden\r\n"
                                         "Content-Length: 0\r\n\r\n");

                write(current_connect, answer_file_size, strlen(answer_file_size));

                close(current_connect);
                continue;
            }

            // file exists and we can read from him
            write(current_connect, "HTTP/1.1 200 OK\r\n", strlen("HTTP/1.1 200 OK\r\n"));

            int fd_file = openat(dir_fd, file_name, 0, "r");
            if (fd_file == -1){
                close(current_connect);
                break;
            }

            // now we get information about size of file
            struct stat stat_file;
            fstat(fd_file, &stat_file);
            char answer_file_size[256];
            int file_size = stat_file.st_size;

            snprintf(answer_file_size, sizeof(answer_file_size), "Content-Length: %d\r\n", file_size);
            strcat(answer_file_size, "\r\n");
            write(current_connect, answer_file_size, strlen(answer_file_size));

            char file_content[4096];
            int num_read_symbols = 0;
            while ((num_read_symbols = read(fd_file, file_content, sizeof(file_content))) > 0) {    // while we can read smt from file
                int write_bytes = write(current_connect, file_content, num_read_symbols); // send it to client
                if (write_bytes == -1){
                    close(current_connect);
                    break;
                }
            }
            close(current_connect); // end connect with client
        }
    }
    // close file descriptors
    close(sock);
    close(dir_fd);

    return 0;
}