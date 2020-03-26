#include <stdio.h>

extern double my_sin(double x);

int main(){
	double x = 0;
	scanf("%lf", &x);

	printf("%lf", my_sin(x));
	
	return 0;

}
