%file 

#include <stdio.h>

extern int f(int a, int b, int c, int d);

int main() {
	printf("%d", f(1, 2, 3, 4));
}
