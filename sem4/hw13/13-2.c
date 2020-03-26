#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    char *file_name = argv[1];

    int fds_pair[2];
    pipe(fds_pair);

    pid_t pid = fork();
    if (0 == pid) {
        dup2(fds_pair[1], 1);
        close(fds_pair[1]);
        char *file_name = argv[1];
        execlp(file_name, "gcc " + file_name, NULL);
    }
    else {
        waitpid(pid, 0 , 0);
        close(fds_pair[1]);
        char buffer[4096];
        while ( 0 != read(fds_pair[0], buffer, sizeof(buffer))) {
            printf("%s", buffer);
        }
    }
    return 0;
}