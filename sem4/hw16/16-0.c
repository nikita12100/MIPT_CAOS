#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void *
thread_func(void *arg)
{
    int value;
    if (scanf("%d", &value) != 1) {
        return NULL;
    }
    pthread_t next_thread;
    pthread_create(&next_thread, NULL, thread_func_sum, 0);
    pthread_join(next_thread, NULL);
    printf("%d\n", value);
    return NULL;
}

int main()
{
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func_sum, 0);
    pthread_join(thread, NULL);
    return 0;
}