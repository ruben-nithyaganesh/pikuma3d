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
#endif
