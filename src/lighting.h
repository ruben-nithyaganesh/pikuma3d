#ifndef LIGHTING
#define LIGHTING

#include <stdlib.h>
#include "vector.h"

typedef struct {
	vec3 global_illumination_direction;
} Lighting;

uint32_t grayscale_of_intensity(float intensity, uint8_t min, uint8_t max);

#endif
