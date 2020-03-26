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

sig_atomic_t MUST_EXIT = 0;
const unsigned int TEXT_SIZE = 4096;

void handle_min(int signum, siginfo_t* info, void* vp)
{
    MUST_EXIT = 1;
}

int main(int argc, char **argv)
{
    uint16_t port_num = strtol(argv[1], NULL, 10);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        perror("socket");
        abort();
    }

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_addr = inet_addr("127.0.0.1"), // localhost
            .sin_port = htons(port_num)
    };


    int my_connect = bind(sock, (const struct sockaddr*)&addr, sizeof(addr));
    if (-1 == my_connect) {
        close(sock);
        return 0;
    }

    listen(sock, SOMAXCONN);

    //-- signal--
    struct sigaction action_int;
    memset(&action_int, 0, sizeof(action_int));
    action_int.sa_flags = SA_SIGINFO;
    action_int.sa_sigaction = handle_min;
    sigaction(SIGRTMIN, &action_int, NULL);
    sigaction(SIGINT, &action_int, NULL);
    //--end signal--

    int current_connect = 0;
    char input_text[TEXT_SIZE ];
    while (!MUST_EXIT){
        action_int.sa_flags = SA_RESTART;
        if( -1 != ( current_connect = accept(sock, NULL, NULL)) && errno == EINTR) {
            continue;
        }
          read(current_connect, &input_text, sizeof(input_text));
        char *file_name;
        sscanf(input_text, "GET %s HTTP/1.1", file_name);

        char *answer;
        unsigned int file_size;
        void *buffer = NULL;
        int descriptor = open(file_name, O_RDONLY);
        char *file_as_is;
        if (descriptor != -1){
            FILE *file = fopen(descriptor, "rb");
            if (file){
                struct stat statistics;
                file_size = statistics.st_size;
                fclose(file);
                answer = "HTTP/1.1 200 OK,";
                fgets(file_as_is, file_size, file);
            }
            else{
                answer = "HTTP/1.1 403 Forbidden,";
            }
            close(descriptor);
         }
        else {
            answer = "HTTP/1.1 404 Not Found";
        }
        write(current_connect, &answer, sizeof(answer));
        char *answer2;
        answer2 = strcat("Content-Length: ", file_size);    // 1) answer
        write(current_connect, &answer2, sizeof(answer2));  // 2) size of file
        write(current_connect, "", 0);      // 3) empty
        write(current_connect, file_as_is, sizeof(file_as_is));

        close(current_connect);

    action_int.sa_flags = SA_SIGINFO;
    }

    close(sock);
}
