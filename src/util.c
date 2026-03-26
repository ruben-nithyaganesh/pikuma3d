#include "util.h"

uint32_t flag_toggle(uint32_t flags, uint32_t f) {
	if(flags & f) {
		return flags & ~f;
	}
	else {
		return flags | f;
	}
}

uint32_t flag_set(uint32_t flags, uint32_t f, uint8_t on) {
	if(on) {
		return flags | f;
	}
	else {
		return flags & ~f;
	}
}

void swap_int(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void swap_float(float *a, float *b) {
	float temp = *a;
	*a = *b;
	*b = temp;
}

int bound(int x, int lower, int upper) {
	if(x < lower) return 0;
	if(x > upper) return upper;
	return x;
}

float mylerp(float a, float b, float t) {
	if (t <= 0.0) return a;
	if (t >= 1.0) return b;
	float lerped = a + (b - a) * t;
	return lerped;
}
