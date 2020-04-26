#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>

typedef double (*function_t)(double);

double* pmap_process(function_t func, const double* in, size_t count) {
    // semaphore to clean synchronize our process
    sem_t* semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE ,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(semaphore, 1, 0);
    // allocate memory
    double* memory = mmap(NULL, sizeof(double) * count, PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    /// process signal
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_IGN;
    action.sa_flags = SA_RESTART | SA_NOCLDWAIT;
    sigaction(SIGCHLD, &action, NULL);
    /// process signal

    unsigned int N = get_nprocs();  // available core numbers
    // how much array elements each process must change
    size_t slide_window = (count - 1) / N + 1; /// N may be 0
    int i = 0;
    pid_t pids[N];
    for(; i < N; ++i) {
        pid_t pid = fork();
        pids[i] = pid;
        if (pid == 0) {
            int current_block = i * slide_window;
            for(int j = 0; j < slide_window; ++j) {
//                printf("old value =%f\n", in[current_block + j]);
                memory[current_block + j] = func(in[current_block + j]); //prepare element
            }
            // to wait main function
            sem_post(semaphore);
            exit(0);
        }
    }
    // now all childrens had finished
    while(i > 0){
        if(sem_wait(semaphore) == 0) { //try!!! to clean semaphore
            i--;    // N times will decrease
        }
    }
    for(int k=0; k<N; ++k) {
        waitpid(pids[k], 0, 0);
    }
    munmap(semaphore, sizeof(sem_t));   // clean memory
    return memory;
}

void pmap_free(double* ptr, size_t count) {
    //clear allocated memory
    munmap(ptr, sizeof(double) * count);
}

//double test_func(double x) {
//    return (x+1);
//}
//int main(int argc, char** argv)
// {
//    int n = 10;
//    double array[n];
//     for(int i=0;i < n;++i)
//         array[i] = 10;
//    double* new_array = pmap_process(test_func, array, 10);
//    for(int i=0;i < n;++i)
//        printf("%f\n", new_array[i]);
//    return 0;
//}