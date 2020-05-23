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
    pthread_mutex_t *mutex;
    // condition to synchronize
    pthread_cond_t *cond_calculate_prime;
    pthread_cond_t *cond_print_prime;
    //interval (a,b)
    long a;
    long b;
    // prime number which will be printed
    long *prime_number;
    //count of prime numbers
    int N;
    int *flag;
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
    int N = sec->N;
    long current = sec->a;

    while(N > 0){
        pthread_mutex_lock(sec->mutex);

        while (*sec->flag) {
            // wait until number is printed
            // unlock mutex to print
            pthread_cond_wait(sec->cond_print_prime, sec->mutex);
            // here mutex is again locked
        }
        if (is_prime(current)) {
            N--;
            *sec->prime_number = current;
            *sec->flag = 1;
            pthread_cond_signal(sec->cond_calculate_prime);
        }
        current++;
        pthread_mutex_unlock(sec->mutex);
    }

    if (N == sec->N){
        *sec->prime_number = -1;    // nothing was found
    }

    return NULL;
}

int main(int argc, char** argv)
{
    // interval
    long A = strtol(argv[1], NULL, 10);
    long B = strtol(argv[2], NULL, 10);
    // count
    int N = strtol(argv[3], NULL, 10);

    int fd[2];
    socketpair(AF_LOCAL, SOCK_STREAM, 0, fd);   // to transfer numbers between threads

    // initialize thread
    pthread_t my_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // to synchronize calculating and printing
    pthread_cond_t cond_calculate_prime = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_print_prime = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // initialize section to transfer between threads
    section my_section;
    my_section.mutex = &mutex;
    my_section.cond_calculate_prime = &cond_calculate_prime;
    my_section.cond_print_prime = &cond_print_prime;
    my_section.a = A;
    my_section.b = B;
    my_section.N = N;
    long *prime_number = malloc(sizeof(long));
    my_section.prime_number = prime_number;
    int flag = 0;
    my_section.flag = &flag;

    pthread_create(&my_thread, &attr, gen_prime, &my_section);
    pthread_attr_destroy(&attr);
    while(N > 0){   // generate N prime numbers
        pthread_mutex_lock(&mutex);
        while(!flag){
            // until prime number not ready we ill wait for it
            // and unlock mutex
            pthread_cond_wait(&cond_calculate_prime, &mutex);
            // here mutex is again locked
        }
        if(*prime_number == -1) {      // nothing was found
            return 0;
        }
        else {
            printf("%li ", *prime_number);
            N--;
            flag = 0;
            pthread_cond_signal(&cond_print_prime);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_join(my_thread, NULL);
    // destroy mutex and condition variable
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_calculate_prime);
    pthread_cond_destroy(&cond_print_prime);
}