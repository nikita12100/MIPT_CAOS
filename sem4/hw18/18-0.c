#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

int main(int argc, char** argv)
{
//    char* sem_name = argv[0];
//    char* mem_name = argv[1];
//    unsigned int N = strtol(argv[2], NULL, 10);
    char sem_name[NAME_MAX];
    char mem_name[NAME_MAX];
    unsigned int N = 0;
    scanf("%s %s %d", sem_name, mem_name, &N);

    sem_t* sem = sem_open(sem_name, 0);
    sem_wait(sem);
    sem_close(sem);

    int mem_fd = shm_open(mem_name, O_RDONLY, 0);
    int* memory = mmap(NULL, N * sizeof(N), PROT_READ, MAP_SHARED, mem_fd, 0);
    close(mem_fd);
    for (int i = 0; i < N; ++i)
        printf("%d ", memory[i]);
    munmap(memory, N * sizeof(N));
}