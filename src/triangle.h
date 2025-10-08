#ifndef TRIANGLE
#define TRIANGLE

#include "vector.h"

typedef struct {
    int a, b, c;
} Face;

typedef struct {
    vec2 points[3];
	float avg_depth;
} Triangle;

extern Face cube_faces[12];

void merge_sort_triangles(Triangle *t, Triangle *scratch, int len);

#endif
