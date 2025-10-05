#ifndef VECTOR
#define VECTOR

// vec2
typedef struct {
    float x, y, z;
} vec2;

extern float vec2_mag_squared(vec2 v);
float vec2_mag(vec2 v);
vec2 vec2_mul(vec2 v, double s);
vec2 vec2_div(vec2 v, double s);
float vec2_dot(vec2 v1, vec2 v2);
vec2 vec2_add(vec2 v, vec2 to_add);
vec2 vec2_sub(vec2 v, vec2 to_sub);
vec2 vec2_invert(vec2 v);

// vec3
typedef struct {
    float x, y, z;
} vec3;

extern float vec3_mag_squared(vec3 v);
float vec3_mag(vec3 v);
vec3 vec3_mul(vec3 v, double s);
vec3 vec3_div(vec3 v, double s);
float vec3_dot(vec3 v1, vec3 v2);
vec3 vec3_add(vec3 v, vec3 to_add);
vec3 vec3_sub(vec3 v, vec3 to_sub);
vec3 vec3_invert(vec3 v);
vec3 vec3_cross_prod(vec3 v1, vec3 v2);

vec3 vec3_rotate_x(vec3 v, float angle);
vec3 vec3_rotate_y(vec3 v, float angle);
vec3 vec3_rotate_z(vec3 v, float angle);

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

#endif
