#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

static void *
thread_inc_5(void* fd)
{
    int my_fd = *((int *) fd);  // to connect ro another thread
    int my_number = 0;      // value to read and prepare and write

    do {
        if( 0 == read(my_fd, &my_number, sizeof(my_number))){   // get val from another thread
            continue;   // if we wait another thread
        }
        if (my_number != 0 && my_number <= 100) {   // if it is good value
            my_number += 5;
            printf("%d ", my_number);
            write(my_fd, &my_number, sizeof(my_number));
        }
        else {  // if another thread finished
            return NULL;    // it will be writen in another thread
        }
    } while (my_number != 0 && my_number <= 100);

    return NULL;
}

static void *
thread_dec_3(void* fd)
{
    int my_fd = *((int *) fd);  // to connect to another thread
    int my_number = 0;

    do {
        if( 0 == read(my_fd, &my_number, sizeof(my_number))){   // nothing to read
            continue;   // if we wait another thread
        }
        if (my_number != 0 && my_number <= 100) {
            my_number -= 3;
            printf("%d ", my_number);
            write(my_fd, &my_number, sizeof(my_number));
        } else {
            return NULL;     // if another thread finished
        }
    } while (my_number != 0 && my_number <= 100);

    return NULL;
}

int main(int argc, char** argv)
{
    int n = strtol(argv[1], NULL, 10);

    // fd 0 for thread 1 and fd 1 for thread 2
    if (n == 0 ){
        printf("0");
        return 0;
    }
    int fd[2];
    socketpair(AF_LOCAL, SOCK_STREAM, 0, fd);

    pthread_t threads[2];
    pthread_create(&threads[0], NULL, thread_dec_3, &fd[0]);
    pthread_create(&threads[1], NULL, thread_inc_5, &fd[1]);

    write(fd[1], &n, sizeof(n));
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    return 0;
}