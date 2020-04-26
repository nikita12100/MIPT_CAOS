#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct {
    long a;
    long b;
    int fd;
    int N;
}section;

// 0 - mean not prime, 1 - mean prime
int is_prime(long n) {
    long n_sqr = (long)sqrt((double)n);
    for(long i = 2; i <= n_sqr; ++i) {
        if(n%i == 0)
            return 0;
    }
    return 1;
}

void* gen_prime(void *arg) {
    section* sec = arg;
    int my_fd = sec->fd;  // to connect ro another thread
    long a = sec->a;    // our section
    long b = sec->b;
    int N = sec->N;
    for(long i = a; i <= b; ++i) {  // check all numbers in section
        if(is_prime(i)) {
            write(my_fd, &i, sizeof(i));    // if prime transfer them to main thread
            N--;
        }
        if(N == 0)
            return NULL;
    }
    long nothing_was_found = -1; // nothing_was_found
    write(my_fd, &nothing_was_found, sizeof(nothing_was_found));    // if prime transfer them to main thread
    return NULL;
}

int main(int argc, char** argv)
{
    long A = strtol(argv[1], NULL, 10);
    long B = strtol(argv[2], NULL, 10);
    int N = strtol(argv[3], NULL, 10);

    int fd[2];
    socketpair(AF_LOCAL, SOCK_STREAM, 0, fd);   // to transfer numbers between threads

    pthread_t my_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    pthread_attr_setguardsize(&attr, 0);

    section my_section;
    my_section.a = A;
    my_section.b = B;
    my_section.fd = fd[0];
    my_section.N = N;
    pthread_create(&my_thread, &attr, gen_prime, &my_section);
    pthread_attr_destroy(&attr);
    long prime_number;
    pthread_join(my_thread, NULL);
    while(N > 0){
        read(fd[1], &prime_number, sizeof(prime_number));
        if(prime_number == -1) {      // nothing was found
            return 0;
        }
        else {
            printf("%li ", prime_number);
            N--;
        }
    }
}