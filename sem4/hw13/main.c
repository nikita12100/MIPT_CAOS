#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    char *comand = argv[1];
    char *input_name = argv[2];

    int input = open(input_name, O_RDONLY);
    dup2(input, 0);
    close(input);

    int fds_pair[2];
    pipe(fds_pair);

    pid_t pid = fork();
    if (0 == pid) {
        dup2(fds_pair[1], 1);
        close(fds_pair[1]);
        execlp(comand, comand, NULL);
    }
    else {
        close(fds_pair[1]); // read finish only when pipe is closed
        uint64_t total_count = 0;
        ssize_t count;
        char buffer[4096];
        while ( (count=read(fds_pair[0], buffer, sizeof(buffer))) > 0) {
            total_count += count;
        }
        waitpid(pid, 0, 0);
        printf("%"PRIu64"\n", total_count);
    }
    return 0;
}