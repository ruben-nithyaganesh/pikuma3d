#ifndef UTIL
#define UTIL
#include <stdint.h>

uint32_t flag_toggle(uint32_t flags, uint32_t f);
uint32_t flag_set(uint32_t flags, uint32_t f, uint8_t on);

void swap_int(int *a, int *b);
void swap_float(float *a, float *b);

int bound(int x, int lower, int upper);

#endif
