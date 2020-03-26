#include <stdio.h>
#include <stdlib.h>

int N;
int *A;
int *B;
int *R;

extern void summ();
extern void everyday795(int x, int y);

int main(){
        scanf("%d", &N);
        A = malloc(sizeof(int) * N);
        B = malloc(sizeof(int) * N);
        R = malloc(sizeof(int) * N);

        for(size_t i = 0;i < N; i++){
                scanf("%d", &A[i]);
        }
        for(size_t i=0; i < N; i++){
                scanf("%d", &B[i]);
        }

        summ();
	for(size_t i= 0; i < N; i++)
		printf("%d ", R[i]);

	int x,y;
        scanf("%d%d", &x, &y);
        everyday795(x, y);

        free(A);
        free(B);
        free(R);

        return 0;
}

