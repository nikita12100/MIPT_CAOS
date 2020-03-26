#include <stdio.h>
#include <stdlib.h>

int summ(int X0, int N, int *x);

int main() {
    int X0,N;
    int *X;

    scanf("%d%d", &X0, &N);
    X = malloc(sizeof(int) * N);
    for(int i=0; i < N; i++)
        scanf("%d", &X[i]);
    int sum = summ(X0, N, X);
    printf("%d\n", sum);

    free(X);
    return 0;
}