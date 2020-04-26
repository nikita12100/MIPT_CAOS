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
#include <netdb.h>

int main(int argc, char** argv)
{
    char* host_name = argv[1];
    char* path = argv[2];
    char* local_file_name = argv[3];

    signal(SIGPIPE, SIG_IGN);

    struct addrinfo addr_hints = {
            .ai_family = AF_INET,
            .ai_socktype = SOCK_STREAM
    };
    struct addrinfo *addr_result = NULL;

    getaddrinfo(host_name, "http", &addr_hints, &addr_result);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, addr_result->ai_addr, addr_result->ai_addrlen);

    FILE* file_to_send = fopen(local_file_name, "r");
    char buff[63000];
    fgets(buff, sizeof(buff), file_to_send);
    char request[65000];
    snprintf(request, sizeof(request),
             "POST %s HTTP/1.1\n"
             "Host:%s\n"
             "Content-Length:%ld\n"
             "Connection: close\n"
             "\n"
             "%s",
             path, host_name, strlen(buff), buff);


    write(sock, request, sizeof(request));

    FILE* in = fdopen(sock, "r");
    char buffer[65536];
    int headers_completed = 0;
    while(fgets(buffer, sizeof(buffer), in)) {
        if(0 == strcmp(buffer, "\n") || 0== strcmp(buffer, "\r\n")){
            headers_completed = 1;
            continue;
        }
        if(headers_completed) {
            printf("%s", buffer);
        }
    }
    fclose(in);

    return 0;
}