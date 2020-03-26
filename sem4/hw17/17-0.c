#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct {
    double *array;
    pthread_mutex_t *mutex;
    pthread_t thread;
    size_t N;
    size_t k;
    size_t index;
}context_t;

void* func(void *arg) {
    const context_t* context = arg;
    const size_t me = context->index;
    const size_t left = 0==me? context->k-1 : me -1;
    const size_t right = context->k-1==me? 0 : me + 1;
    const double  add_left = 0.99;
    const double  add_me = 1;
    const double add_right = 1.01;
    for (size_t i=0; i < context->N; ++i) {
        pthread_mutex_lock(context->mutex);
        context->array[left] += add_left;
        context->array[me] += add_me;
        context->array[right] += add_right;
        pthread_mutex_unlock(context->mutex);
    }
    return 0;
}

int main(int argc, char** argv)
{
    int N = strtol(argv[1], NULL, 10);
    int k = strtol(argv[2], NULL, 10);
    context_t threads[k];
    double array[k];
    memset(array, 0, sizeof(array));
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    pthread_attr_setguardsize(&attr, 0);
    for(size_t i=0; i < k; ++i) {
        threads[i].k = k;
        threads[i].N = N;
        threads[i].index = i;
        threads[i].mutex = &mutex;
        threads[i].array = array;
        pthread_create(&threads[i].thread, &attr, func, &threads[i]);
    }
    pthread_attr_destroy(&attr);
    for(size_t i=0; i < k; ++i) {
        pthread_join(threads[i].thread, NULL);
    }
    for(size_t i=0; i < k; ++i) {
        printf("%.10g\n", array[i]);
    }
}