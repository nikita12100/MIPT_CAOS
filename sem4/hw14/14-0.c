#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    uint16_t port_num = strtol(argv[2], NULL, 10);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        perror("socket");
        exit(1);    // close program
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_addr = inet_addr(argv[1]),
            .sin_port = htons(port_num)
    };

    int send_num, recv_num;

    int my_connect = connect(sock, (const struct sockaddr*)&addr, sizeof(addr));
    if (-1 == my_connect) {
        close(sock);
        return 0;
    }

    while (scanf("%d", &send_num) > 0) {
        write(sock, &send_num, sizeof(send_num));
        if (0 == read(sock, &recv_num, sizeof(recv_num))){
            close(sock);
            return 0;
        }
        printf("%d\n", recv_num);
    }
    shutdown(sock, SHUT_RDWR);  // to correctly close connection (tcp)
    close(sock);
}
