#ifndef TRIANGLE
#define TRIANGLE

#include <stdlib.h>
#include "vector.h"
#include "texture.h"

typedef struct {
    int a, b, c;
	int a_n, b_n, c_n;
	int a_uv, b_uv, c_uv;
} Face;

typedef struct {
    vec4 points[3];
	tex2d tex_coords[3];
	uint32_t col;
	float intensity;
	float avg_depth;
} Triangle;

// extern Face cube_faces[12];

void merge_sort_triangles(Triangle *t, Triangle *scratch, int len);

#endif
