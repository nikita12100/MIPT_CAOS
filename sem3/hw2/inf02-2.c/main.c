#define _GNU_SOURCE
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
            printf("!!!!!!!!!!!!!");
    char* file_name = "test.nk";//argv[1];
    int n = 5;//atoi(argv[2]);
    int w = 5;//atoi(argv[3]);
    int fd = open(file_name, O_CREAT || O_WRONLY);
    if(fd == -1) {
        close(fd);
        return 1;
    }
    struct stat st;
    fstat(fd, &st);
    if (-1 == lseek(fd, w*n*n + n, SEEK_SET)) {
        close(fd);
        return 2;
    }
    write(fd, " ", 1);
    char* output = mmap(NULL, st.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
    int len = st.st_size;
    char* entry = output;
    int result[n][n];
    int i = 1, j, k, p = n / 2;
    for (k = 1; k <= p; k++)
    {
        for (j = k - 1; j < n - k + 1; j++) result[k - 1][j] = i++;
        for (j = k; j < n - k + 1; j++) result[j][n - k] = i++;
        for (j = n - k - 1; j >= k - 1; --j) result[n - k][j] = i++;
        for (j = n - k - 1; j >= k; j--) result[j][k - 1] = i++;
    }
    if (n % 2 == 1) result[p][p] = n * n;

    int kind = w - 1;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            int temp = result[i][j];
            int indx = kind;
            while (temp / 10 > 0) {
                output[indx] = temp % 10 + '0';
                indx--;
            }
            while (indx > kind - w) {
                output[indx] = ' ';
                indx--;
            }
            //printf(" %4d ", result[i][j]);
            //if (j == n - 1) printf("\n");
            kind += w;
        }
    }
    munmap(output, st.st_size);
    close(fd);
    return 0;
}