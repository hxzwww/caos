#include <inttypes.h>

typedef enum {
	PlusZero      = 0x00,
	MinusZero     = 0x01,
	PlusInf       = 0xF0,
	MinusInf      = 0xF1,
	PlusRegular   = 0x10,
	MinusRegular  = 0x11,
	PlusDenormal  = 0x20,
	MinusDenormal = 0x21,
	SignalingNaN  = 0x30,
	QuietNaN      = 0x31
} float_class_t;

extern float_class_t classify(double *value_ptr) {
	union {
		double double_view;
		uint64_t uint64_t_view;
	} number;
	number.double_view = *value_ptr;
	uint64_t converted_value = number.uint64_t_view;
	uint64_t sign = (converted_value >> 63) & 1;
	uint64_t exp = (converted_value >> 52) & ((1 << 11) - 1);
	uint64_t mant = converted_value & (((uint64_t)1 << 52) - 1);
	if (exp == 0) {
		if (mant == 0) {
			return (sign ? MinusZero : PlusZero);
		} else {
			return (sign ? MinusDenormal : PlusDenormal);
		}
	} else {
		if (exp == ((1 << 11) - 1)) {
			if (mant == 0) {
				return (sign ? MinusInf : PlusInf);
			} else {
				return ((mant >> 51) ? QuietNaN : SignalingNaN);
			}
		} else {
			return (sign ? MinusRegular : PlusRegular);
		}
	}
}
