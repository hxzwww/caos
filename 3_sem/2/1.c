#include <stdint.h>

ITYPE sum(ITYPE first, ITYPE second) {
	while (second) {
		ITYPE copy = first;
		first ^= second;
		second &= copy;
		second <<= 1;
	}
	return first;
}

extern void mul(ITYPE first, ITYPE second, ITYPE *res, int *CF) {
	CF[0] = 0;
	ITYPE mul = 0;
	int c = 0;
	while (second) {
		if (second & 1) {
			ITYPE auf = first << c;
			mul = sum(mul, auf);
			if (sum(mul, auf) < mul || sum(mul, auf) < auf) {
				CF[0] = 1;
			}
		}
		++c;
		second >>= 1;
	}
	res[0] = mul;

}
