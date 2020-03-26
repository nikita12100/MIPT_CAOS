#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

volatile sig_atomic_t signal_pid = 0;
volatile sig_atomic_t n = 0;

void handle_min(int signum, siginfo_t* info, void* vp)
{
    n = info->si_int;
    signal_pid = info->si_pid;
    if (n == 0) {
        return;
    }
    n--;
}

int main(int argc, char** argv)
{
    // printf("pid=%d\n", getpid());
    // fflush(stdout);
    struct sigaction action_int;
    memset(&action_int, 0, sizeof(action_int));
    action_int.sa_flags = SA_SIGINFO;
    action_int.sa_sigaction = handle_min;
    sigaction(SIGRTMIN, &action_int, NULL);

    // int sigqueue(pid_t pid, int signum, const union sigval value);
    do {
        pause();

        union sigval value;
        value.sival_int = n;
        sigqueue(signal_pid, SIGRTMIN, value);
    } while (n != 0);

    return 0;
}