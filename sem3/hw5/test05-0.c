#include <stdio.h>
#include <stdlib.h>

extern double calc(double a, double b, double c, int d);

extern void vector_sum(int n, const float *a, const float *b, float *r);


int main(){
	double a,b,c;
	int d,n = 0;

	float *x,*y;//,*z;

	scanf("%lf%lf%lf%d", &a, &b, &c, &d);

	printf("%lf", calc(a, b, c, d));

	scanf("%d", &n);

	__attribute__(aligned 32) float z[4] = {1, 2, 3, 4};// alligne
	x = malloc(n*sizeof(float));	// alligned_alloc
	y = malloc(n*sizeof(float));
	//z = malloc(n*sizeof(float));

	for(int i=0; i < n; i++)
		scanf("%f%f", &x[i], &y[i]);

	vector_sum(n, x, y ,z);

	for(int i=0; i < n; i++)
		printf("%f", z[i]);

	free(x);
	free(y);
	free(z);

	return 0;
}
