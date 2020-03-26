#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    char *cmd1 = argv[1];
    char *cmd2 = argv[2];

    int fds_pair[2];
    pipe(fds_pair);

    pid_t pid = fork();
    if (0 == pid) { //it is cmd1
        dup2(fds_pair[1], 1);
        close(fds_pair[1]);
        execlp(cmd1, cmd1, NULL);
    }
    else {
        close(fds_pair[1]);
        pid_t pid2 = fork();
        if (0 == pid2) {    // it is cmd2
            dup2(fds_pair[0],0 );
            close(fds_pair[0]);
            execlp(cmd2, cmd2, NULL);
        }
        else {
            waitpid(pid, 0, 0); // not to wait cmd1
            waitpid(pid2, 0, 0);
        }
    }
    return 0;
}