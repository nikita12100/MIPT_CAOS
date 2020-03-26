#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0;
volatile sig_atomic_t must_exit = 0;

void handle_sigint(int signum)
{
    counter++;
    printf("%d\n", counter);
    fflush(stdout);
}

void handle2_sigint(int signum)
{
    counter *= -1;
    printf("%d\n", counter);
    fflush(stdout);
}

void handle3_sigint(int signum)
{
    must_exit = 1;
}

int main()
{
    struct sigaction action_int;
    memset(&action_int, 0, sizeof(action_int));
    action_int.sa_handler = handle_sigint;
    action_int.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &action_int, NULL);

    struct sigaction action2_int;
    memset(&action2_int, 0, sizeof(action2_int));
    action2_int.sa_handler = handle2_sigint;
    action2_int.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &action2_int, NULL);

    struct sigaction action3_int;
    memset(&action3_int, 0, sizeof(action3_int));
    action3_int.sa_handler = handle3_sigint;
    action3_int.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &action3_int, NULL);

    printf("%d\n", getpid());
    scanf("%d", &counter);

    fflush(stdout);
    while (!must_exit) {
        pause();
    }
}
