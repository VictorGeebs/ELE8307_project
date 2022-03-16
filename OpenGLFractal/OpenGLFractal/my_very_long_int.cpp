#include "my_very_long_int.h"
#include "math.h"

typedef unsigned int my_very_long_int[NB_32BITS_WORDS];

void my_addition(my_very_long_int RESULT, my_very_long_int X, my_very_long_int Y) {
	unsigned int carry = 0;
	for (int i = 0; i < NB_32BITS_WORDS; i++) {
		unsigned int res = X[i] + carry; carry = (res < carry) ? 1 : 0;
		res += Y[i]; carry |= (res < Y[i]) ? 1 : 0;
		RESULT[i] = res;
	}
}

void my_subtraction(my_very_long_int RESULT, my_very_long_int X, my_very_long_int Y) {
	unsigned int carry = 1;
	for (int i = 0; i < NB_32BITS_WORDS; i++) {
		unsigned int res = X[i] + carry; carry = (res < carry) ? 1 : 0;
		unsigned int negY = (Y[i] ^ 0xFFFFFFFF);
		res += negY; carry |= (res < negY) ? 1 : 0;
		RESULT[i] = res;
	}
}

void my_copy(my_very_long_int RESULT, my_very_long_int X) {
	for (int i = 0; i < NB_32BITS_WORDS; i++) {
		unsigned int res = X[i];
		RESULT[i] = res;
	}
}

void my_negate(my_very_long_int RESULT, my_very_long_int X) {
	unsigned int carry = 1;
	for (int i = 0; i < NB_32BITS_WORDS; i++) {
		unsigned int res = (X[i] ^ 0xFFFFFFFF) + carry;
		carry = (res < carry) ? 1 : 0;
		RESULT[i] = res;
	}
}

unsigned int my_positive(my_very_long_int X) {
	return (X[NB_32BITS_WORDS - 1] & 0x80000000) == 0;
}


void my_asr(my_very_long_int RESULT, my_very_long_int X, int shift_value, int LOCAL_NB_32BITS_WORDS) {
	while (shift_value > 0) {
		for (int i = 0; i < LOCAL_NB_32BITS_WORDS - 1; i++) {
			RESULT[i] = X[i] >>= 1;
			if (RESULT[i + 1] & 1) RESULT[i] |= 0x80000000;
		}
		RESULT[LOCAL_NB_32BITS_WORDS - 1] = X[LOCAL_NB_32BITS_WORDS - 1] >>= 1;
		if (RESULT[LOCAL_NB_32BITS_WORDS - 1] & 0x40000000) RESULT[LOCAL_NB_32BITS_WORDS - 1] |= 0x80000000;
		shift_value--;
	}
}

void my_multiply_shift(my_very_long_int temp_RESULT, my_very_long_int temp_X, my_very_long_int temp_Y, int shift_value) {
	unsigned int RESULT[2 * NB_32BITS_WORDS];
	int sign = 0;
	my_very_long_int X, Y;

	if (!my_positive(temp_X)) {
		sign = 1 - sign;
		my_negate(X, temp_X);
	}
	else my_copy(X, temp_X);

	if (!my_positive(temp_Y)) {
		sign = 1 - sign;
		my_negate(Y, temp_Y);
	}
	else my_copy(Y, temp_Y);
	int x, y;

	for (x = 0; x < 2 * NB_32BITS_WORDS; x++) {
		RESULT[x] = 0;
	}
	for (x = 0; x < NB_32BITS_WORDS; x++) {
		unsigned long long opx = X[x];
		unsigned long accumulator = 0;

		for (y = 0; y < NB_32BITS_WORDS; y++) {
			if (x + y == NB_32BITS_WORDS) break;
			unsigned long long opy = Y[y];
			unsigned long long result = opx * opy + accumulator + RESULT[x + y];
			RESULT[x + y] = result & 0xFFFFFFFF;
			accumulator = result >> 32;
		}
		RESULT[x + y] = accumulator;
	}
	my_asr(RESULT, RESULT, shift_value, 2 * NB_32BITS_WORDS);
	if (sign) my_negate(temp_RESULT, RESULT);
	else my_copy(temp_RESULT, RESULT);
}

void my_init_from_double(my_very_long_int X, double d) {//Max 64 bits
	long long ll = (d * (1LL << my_verly_long_precision));
	for (int i = 0; i < NB_32BITS_WORDS; i++) {
		X[i] = ll & 0xFFFFFFFF;
		ll >>= 32;
	}
}

double my_to_double(my_very_long_int X) {//Max 64 bits
	double result = 0;
	double sign = 1;
	my_very_long_int temp_X;

	if (my_positive(X)) my_copy(temp_X, X);
	else {
		my_negate(temp_X, X);
		sign = -1;
	}

	for (int i = NB_32BITS_WORDS - 1; i >= 0; i--) {
		result *= (1LL << 32);
		result += temp_X[i];
	}

	return sign * result * pow(2, -my_verly_long_precision);
}

