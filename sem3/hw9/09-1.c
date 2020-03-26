#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int counter = 1;
    pid_t pid;
    do {
        pid = fork();
        if (pid > 0) {
            printf("%d ", counter);
            exit;
	    // int status;
            // waitpid(pid, &status, 0);
        }
        if (counter == atoi(argv[1])) {
            printf("\n");
            break;
        }
        counter++;
    } while (0 == pid);

}
