#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

enum constants {
	last_bit = 61,
	digits_start = 48,
	digits_end = 57,
	uppercase_start = 65,
	uppercase_end = 90,
	lowercase_start = 97,
	lowercase_end = 122,
	digits_shift = 0,
	uppercase_shift = 10,
	lowercase_shift = 36
};

int index_in_bit_mask(int symbol) {
	if (symbol >= digits_start && symbol <= digits_end) {
		return last_bit - symbol + digits_start - digits_shift;
	}
	if (symbol >= uppercase_start && symbol <= uppercase_end) {
		return last_bit - symbol + uppercase_start - uppercase_shift;
	}
	if (symbol >= lowercase_start && symbol <= lowercase_end) {
		return last_bit - symbol + lowercase_start - lowercase_shift;
	}
	return last_bit;
}

bool count(int symbol, uint64_t* result, uint64_t* current_value) {
	if (symbol == '&') {
		*result &= *current_value;
		return 1;
	}
	if (symbol == '|') {
		*result |= *current_value;
		return 1;
	}
	if (symbol == '^') {
		*result ^= *current_value;
		return 1;
	}
	if (symbol == '~') {
		*result = ~(*result);
		return 1;
	}
	*current_value |= ((uint64_t)1 << index_in_bit_mask(symbol));
	return 0;
}

int main() {
	uint64_t start_result = 0, start_current_value = 0;
	uint64_t* result = &start_result;
	uint64_t* current_value = &start_current_value;
	int read = 0;
	while (read != EOF) {
		read = getchar();
		if (count(read, result, current_value)) {
			*current_value = 0;
		}
	}
	for (int i = 0; i < 62; ++i) {
		if (((*result) >> (61 - i)) & 1) {
			if (i >= digits_shift && i <= uppercase_shift - 1) {
				printf("%c", i + digits_start);
			} else if (i >= uppercase_shift && i <= lowercase_shift - 1) {
				printf("%c", i + uppercase_start - uppercase_shift);
			} else {
				printf("%c", i + lowercase_start - lowercase_shift);
			}
		}
	}
}
