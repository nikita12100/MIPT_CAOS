#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


static void *
thread_func_sum(void *arg)
{
    int value = 0;
    long sum = 0;   // to cast to int from smaller integer type
    while (scanf("%d", &value) == 1) {
        sum += value;
    }
    return (int*) sum;  // must be void**
}

int main(int argc, char** argv)
{
    unsigned int N = strtol(argv[1], NULL, 10);
    pthread_attr_t thread_attr; // attribute for all threads
    pthread_attr_init(&thread_attr);
    pthread_attr_setstacksize(&thread_attr, _SC_THREAD_STACK_MIN);
    //pthread_attr_setguardsize(thread_attr, 0);
    pthread_t threads[N];
    int part_sum = 0;
    int sum = 0;    // our answer
    for(unsigned int i = 0; i < N; ++i) {
        pthread_create(&threads[i], &thread_attr, thread_func_sum, 0);
        //pthread_join(threads[i], (void*) &part_sum);
        //sum += part_sum;
    }
    for (unsigned int i = 0; i < N; ++i) {  // firstly create and now start them
        pthread_join(threads[i], (void*) &part_sum);
        sum += part_sum;
    }
    pthread_attr_destroy(&thread_attr);
    printf("%d\n", sum);
    return 0;
}