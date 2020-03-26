#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    float x;
    int y;

    scanf("%f", &x);
    scanf("%x", &y);
    printf("%.3f", x + y + strtol(argv[1], NULL, 27));

    return 0;
}