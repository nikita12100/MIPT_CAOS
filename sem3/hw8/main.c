#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    char* file_name = argv[1];
    char* pattern = argv[2];
    int fd = open(file_name, O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char* constest = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char* entry = constest;
    while (NULL !=
           (entry = memmem(entry, st.st_size, pattern, strlen(pattern)))) {
        size_t diff = entry - constest;
        printf("%lu ", diff);
        entry++;
        // getchar();
    }
    printf("\n");
    munmap(constest, st.st_size);
    close(fd);
    return 0;
}
