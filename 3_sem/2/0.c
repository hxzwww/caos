#include <stdint.h>

extern void sum(ITYPE first, ITYPE second, ITYPE *res, int *CF) {
	CF[0] = 0;
	ITYPE f_c = first, s_c = second;
	while (second > 0) {
		ITYPE copy = first;
		first ^= second;
		second &= copy;
		second <<= 1;
	}
	res[0] = first;
	if (first < f_c || first < s_c) {
		CF[0] = 1;
	}
}
