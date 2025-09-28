#ifndef TRIANGLE
#define TRIANGLE

#include "vector.h"

typedef struct {
    int a, b, c;
} Face;

typedef struct {
    vec2 points[3];
} Triangle;

extern Face cube_faces[12];


#endif