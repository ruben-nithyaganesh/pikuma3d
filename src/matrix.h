#ifndef MATRIX
#define MATRIX

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
// mat3
typedef struct {
    float e00, e01, e02;
    float e10, e11, e12;
    float e20, e21, e22;
} mat3;

mat3 mat3_add(mat3 m1, mat3 m2);
mat3 mat3_sub(mat3 m1, mat3 m2);
mat3 mat3_mul(mat3 m1, mat3 m2);

// mat4
typedef struct {
    float e00, e01, e02, e03;
    float e10, e11, e12, e13;
    float e20, e21, e22, e23;
    float e30, e31, e32, e33;
} mat4;

char* mat4_tostring(mat4 *m);
void mat4_load_from_text(const char* filename, mat4 *m);
mat4 mat4_mul(mat4 m1, mat4 m2);

mat4 mat4_identity();
mat4 mat4_scale_matrix(float sx, float sy, float sz);
mat4 mat4_translation_matrix(float tx, float ty, float tz);
mat4 mat4_rotation_matrix_x(float a);
mat4 mat4_rotation_matrix_y(float a);
mat4 mat4_rotation_matrix_z(float a);
mat4 mat4_projection_matrix(float fov, float aspect, float znear, float zfar);

vec4 mat4_project_and_normalise(vec4 v, mat4 proj);
vec4 mat4_mul_vec4(mat4 m, vec4 v);
#endif
