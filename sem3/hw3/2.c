#include <stdlib.h>
#include <stdio.h>

int main(){
	char *p = realloc(0, 4);
	fputc(p[0], stdout);
	free(p);
	return 0;
}
