#ifndef TRIANGLE
#define TRIANGLE

#include "vector.h"

typedef struct {
    int a, b, c;
} Face;

typedef struct {
    vec2 points[3];
} Triangle;

#define N_CUBE_FACES 12
extern Face cube_faces[N_CUBE_FACES];


#endif