#include <stdio.h>
#include <stdlib.h>


extern void mergesort(int from, int to, const int *in, int *out);

int main(){
	int n = 0;
	scanf("%d",&n);
	
	int *raw; 
	raw = malloc(n*sizeof(int));
	int *res;
       	res = malloc(n*sizeof(int));
	for(int i=0; i<n; i++)
		scanf("%d", &raw[i]);

	mergesort(0, n, raw, res);

	for(int i=0; i<n; i++)
		printf("%d ", res[i]);

	free(raw);
	free(res);

	return 0;
}
