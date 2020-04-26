#include <stdio.h>
#include <dlfcn.h>

extern double function(double argument);

int main(int argc, char** argv) {
    char* handle = dlopen(argv[0], 0);

    double x = 0;
    while( EOF != scanf("%lf", &x)){
        printf("%.3f ", function(x));
    }

    printf("Hello, World!\n");
    dlclose(handle);
    return 0;
}