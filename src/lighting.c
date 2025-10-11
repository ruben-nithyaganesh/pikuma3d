#include "lighting.h"

uint32_t grayscale_of_intensity(float intensity, uint8_t min, uint8_t max) {
	uint8_t rgb_val = min + intensity * (max - min);
	return 0xFF000000 |
	(rgb_val << 16) |
	(rgb_val << 8) |
	(rgb_val);
}
