#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

volatile sig_atomic_t n = 222;

void handle_min(int signum, siginfo_t* info, void* vp)
{
    n = info->si_int;
    printf("N=%d\n", n);
}

int main()
{
    printf("my pid = %d\n", getpid());
    fflush(stdout);
    pid_t pid;
    scanf("%d", &pid);
    union sigval value;
    value.sival_int = 23;
    sigqueue(pid, SIGRTMIN, value);


    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handle_min;
    sigaction(SIGRTMIN, &action, NULL);

    kill(pid, SIGRTMIN);
    pause();

    printf("%d", n);
}