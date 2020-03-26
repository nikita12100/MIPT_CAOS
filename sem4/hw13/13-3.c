#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
    int cmd_count = argc - 1;

    int fds_pairs[2][2];
    pipe(fds_pairs[0]);
    pipe(fds_pairs[1]);

    fcntl(fds_pairs[0][0], F_SETFL, O_NONBLOCK);
    fcntl(fds_pairs[1][0], F_SETFL, O_NONBLOCK);

    pid_t pid[cmd_count];
    for (int i = 0; i < cmd_count; i++){
        if (i == 0) {
            pid[i] = fork();
            if (0 == pid[i]) {
                dup2(fds_pairs[0][1], 1);
                close(fds_pairs[0][1]);
                execlp(argv[i + 1], argv[i + 1], NULL);
            }
        }
        else if (i > 0 && i < cmd_count - 1) {
            waitpid(pid[i - 1], 0, 0);
            pid[i] = fork();
            if (0 == pid[i]) {
                dup2(fds_pairs[abs(i%2 - 1)][0], 0);   // i=1 -> fds to read 0
                close(fds_pairs[abs(i%2 - 1)][0]);
                dup2(fds_pairs[i%2][1], 1);
                close(fds_pairs[i%2][1]);
                execlp(argv[i + 1], argv[i + 1], NULL);
            }
            else {
                waitpid(pid[i], 0, 0);
            }
        }
        else if (i == cmd_count - 1){
            waitpid(pid[i - 1], 0, 0);
            close(fds_pairs[abs(i%2 - 1)][1]);
            dup2(fds_pairs[abs(i%2 - 1)][0], 0);   // i=1 -> fds to read 0
            close(fds_pairs[abs(i%2 - 1)][0]);
            pid[i] = fork();
            if (0 == pid[i]) {
                execlp(argv[i + 1], argv[i + 1], NULL);
            }
        }
    }
    waitpid(pid[cmd_count - 1], 0, 0);
    return 0;
}