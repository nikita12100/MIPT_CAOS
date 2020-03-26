#define _GNU_SOURCE
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    char* file_name = argv[1];
    struct stat st;
    int fd = open(file_name, O_RDONLY);
    fstat(fd, &st);
    char* input = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (input == NULL || st.st_size == 0) {
        close(fd);
        return 2;
    }
    char* entry = input;
    uint32_t next_pointer = 0;
    int len = st.st_size;
    while (NULL != 
           (entry = memmem(entry, len, input + next_pointer, sizeof(int)))){
        int val_pos = entry - input;
        entry++;
        len = st.st_size - val_pos;
        printf("%d", input[val_pos]);
        next_pointer += sizeof(int);
    } 
    // do {
    // memmove(&value, &input[next_pointer], sizeof(int));
    // printf("%d ", value);
    // memcpy(&next_pointer, &input[next_pointer + sizeof(int)], sizeof(int));
    //} while (NULL != next_pointer);
    printf("\n");
    munmap(input, len);
    close(fd);
    return 0;
}
