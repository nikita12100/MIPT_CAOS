#include <stdio.h>
#include <stdlib.h>

extern void summ(int N, const int *A, const int *B, int *R);

int main(){
	int n;
	int *a;	
	int *b;
	int *r;

	scanf("%d", &n);

	a = malloc(sizeof(int)*n);
	b = malloc(sizeof(int)*n);
	r = malloc(sizeof(int)*n);
	for(int i=0; i < n;i++)
		scanf("%d", &a[i]);
	for(int i=0; i < n; i++)
		scanf("%d", &b[i]);

	summ(n, a, b, r);
	for(int i=0; i < n; i++)
		printf("%d ", r[i]);
	
	free(a);
	free(b);
	free(r);

	return 0;
	}
