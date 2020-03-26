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
    int n = (int)argv[2];
    int w = (int)argv[3];

    int res[n][w];
    int n = 1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < w; j++) {
	    res[i][j] = n;
	    n++;
	}
    
    }
}
