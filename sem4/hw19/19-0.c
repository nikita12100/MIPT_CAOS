#include <stdio.h>
#include <dlfcn.h>

//typedef double function(double argument);
typedef double (*fun_t)(double);

int main(int argc, char** argv) {
    void* library = dlopen(argv[1], RTLD_NOW);
    fun_t func = dlsym(library, argv[2]);
    double x = 0;
    while( EOF != scanf("%lf", &x)){
        printf("%.3f ", func(x));
    }

    dlclose(library);
    return 0;
}