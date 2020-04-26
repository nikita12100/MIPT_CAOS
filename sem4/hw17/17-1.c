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
    long *prime_number;
    int N;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond_var_1;
    pthread_cond_t *cond_var_2;
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
            pthread_cond_wait(sec->cond_var_2, sec->mutex);
        }
        if (is_prime(current)) {
            N--;
            *sec->prime_number = current;
            *sec->flag = 1;
            pthread_cond_signal(sec->cond_var_1);
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
    pthread_cond_t cond_var_1 = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_var_2 = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    section my_section;
    my_section.a = A;
    my_section.b = B;
    my_section.N = N;
    my_section.mutex = &mutex;
    my_section.cond_var_1 = &cond_var_1;
    my_section.cond_var_2 = &cond_var_2;
    long *prime_number = malloc(sizeof(long));
    my_section.prime_number = prime_number;
    int flag = 0;
    my_section.flag = &flag;

    pthread_create(&my_thread, &attr, gen_prime, &my_section);
    pthread_attr_destroy(&attr);
    while(N > 0){
        pthread_mutex_lock(&mutex);
        while(!flag){
            pthread_cond_wait(&cond_var_1, &mutex);
        }
        if(*prime_number == -1) {      // nothing was found
            return 0;
        }
        else {
            printf("%li ", *prime_number);
            N--;
            flag = 0;
            pthread_cond_signal(&cond_var_2);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_join(my_thread, NULL);
}