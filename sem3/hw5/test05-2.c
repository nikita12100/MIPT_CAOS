#include <stdio.h>
#include <stdlib.h>

extern float dot_product(int N, const float *A, const float *B);

int main(){
	int n=0;
	scanf("%d", &n);

	float *a, *b;
	a = malloc(n*sizeof(float));
	b = malloc(n*sizeof(float));

	for(int i=0;i<n;i++)
		scanf("%f",&a[i]);
	for(int i=0;i<n;i++)
                scanf("%f",&b[i]);

	printf("%f", dot_product(n, a, b));

	return 0;
}
