#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    uint64_t summ = 0;
    char file_name[PATH_MAX];
    struct stat buff_stat;
    char* nl;
    while (fgets(file_name, sizeof(file_name), stdin)) {
        nl = memchr(file_name, '\n', sizeof(file_name));
        if (nl) {
            *nl = '\0';
        }
        if (-1 != lstat(file_name, &buff_stat)) {
            if (S_ISREG(buff_stat.st_mode)) {
                summ += buff_stat.st_size;
            }
        }
    }
    printf("%" PRIu64 "\n", summ);
    return 0;
}
