#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


typedef struct {
    sem_t request_ready; // begin value 0
    sem_t response_ready; // begin value 0
    char func_name[20];
    double value;
    double result;
} shared_data_t;

// function in shared_data_t. len(name) < 20
typedef double (*function_name)(double arg1);

int main(int argc, char** argv)
{
    void* library = dlopen(argv[1], RTLD_NOW);

    char* shared_memory = "/diht82206"; // unique name
    int fd = shm_open(shared_memory,  O_CREAT | O_RDWR, 0644);
    if (fd == -1){
        dlclose(library);
        exit(1);
    }
    // cause the regular file name by fd
    ftruncate(fd, sizeof(shared_data_t));
    // now put our structure in shared memory
    shared_data_t* memory = mmap(NULL, sizeof(shared_data_t),PROT_WRITE | PROT_READ, MAP_SHARED,fd, 0);
    if(memory == MAP_FAILED){
        shm_unlink(shared_memory);
        dlclose(library);
        exit(1);
    }
    // initialize semaphores
    sem_t* semaphore[2];
    semaphore[0] = sem_open("first_sem", O_CREAT, 0644, 0);
    memory->request_ready = *semaphore[0];
    semaphore[1] = sem_open("second_sem", O_CREAT, 0644, 0);
    memory->response_ready = *semaphore[1];

    // print name -- it mean we are ready
    /// printf("%s", shared_memory)
    puts(shared_memory);
    fflush(stdout); // recommended

    //when other part will be ready -- then will unlock semaphore request_ready
    //we must do function with function_name
    // empty function_name mean end
    sem_wait(&memory->request_ready);
    while (strlen(memory->func_name) > 0) {
        function_name function = dlsym(library, memory->func_name); // find our func in library
        memory->result = function(memory->value);   // get result
//        printf("%lf\n", memory->value);
//        printf("%lf\n", memory->result);
        sem_post(&memory->response_ready);  //block semaphare -- mean we are ready
        sem_wait(&memory->request_ready);   // and wait again
    }

    sem_close(semaphore[0]);
    sem_close(semaphore[1]);
    munmap(memory, sizeof(shared_data_t));
    shm_unlink(shared_memory);
    dlclose(library);
    return 0;
}