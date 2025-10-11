#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

//vec2
float vec2_mag_squared(vec2 v) {
    return (
        (v.x * v.x) +
        (v.y * v.y)
    );
}

float vec2_mag(vec2 v) {
    return sqrt(vec2_mag_squared(v));
}

vec2 vec2_mul(vec2 v, double s) {
    vec2 a;
    a.x = v.x * s;
    a.y = v.y * s;
    return a;
}

vec2 vec2_div(vec2 v, double s) {
    vec2 a;
    a.x = v.x / s;
    a.y = v.y / s;
    return a;
}

float vec2_dot(vec2 v1, vec2 v2) {
    float dot = v1.x * v2.x + v1.y * v2.y;
    return dot;
}

vec2 vec2_add(vec2 v, vec2 to_add) {
    vec2 a;
    a.x = v.x + to_add.x;
    a.y = v.y + to_add.y;
    return a;
}

vec2 vec2_sub(vec2 v, vec2 to_sub) {
    vec2 a;
    a.x = v.x - to_sub.x;
    a.y = v.y - to_sub.y;
    return a;
}

vec2 vec2_invert(vec2 v) {
    vec2 a;
    a.x = -v.x;
    a.y = -v.y;
    return a;
}


// vec3
float vec3_mag_squared(vec3 v) {
    return (
        (v.x * v.x) +
        (v.y * v.y) +
        (v.z * v.z)
    );
}

float vec3_mag(vec3 v) {
    return sqrt(vec3_mag_squared(v));
}

vec3 vec3_mul(vec3 v, double s) {
    vec3 a;
    a.x = v.x * s;
    a.y = v.y * s;
    a.z = v.z * s;
    return a;
}

vec3 vec3_div(vec3 v, double s) {
    vec3 a;
    a.x = v.x / s;
    a.y = v.y / s;
    a.z = v.z / s;
    return a;
}

float vec3_dot(vec3 v1, vec3 v2) {
    float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    return dot;
}

vec3 vec3_add(vec3 v, vec3 to_add) {
    vec3 a;
    a.x = v.x + to_add.x;
    a.y = v.y + to_add.y;
    a.z = v.z + to_add.z;
    return a;
}

vec3 vec3_sub(vec3 v, vec3 to_sub) {
    vec3 a;
    a.x = v.x - to_sub.x;
    a.y = v.y - to_sub.y;
    a.z = v.z - to_sub.z;
    return a;
}

vec3 vec3_invert(vec3 v) {
    vec3 a;
    a.x = -v.x;
    a.y = -v.y;
    a.z = -v.z;
    return a;
}

vec3 vec3_normalise(vec3 v) {
	float mag = vec3_mag(v);
	float m = (1.0 / mag);
	vec3 normed;
	normed.x = v.x * m;
	normed.y = v.y * m;
	normed.z = v.z * m;
	return normed;
}

vec3 vec3_rotate_x(vec3 v, float angle){
    vec3 rotated = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated;
}

vec3 vec3_rotate_y(vec3 v, float angle){
    vec3 rotated = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated;
}

vec3 vec3_rotate_z(vec3 v, float angle){
    vec3 rotated = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated;
}

vec3 vec3_cross_prod(vec3 v1, vec3 v2) {
	vec3 cross_prod;
	cross_prod.x = v1.y * v2.z - v1.z * v2.y;
	cross_prod.y = v1.z * v2.x - v1.x * v2.z;
	cross_prod.z = v1.x * v2.y - v1.y * v2.x;
	return cross_prod;
}


vec4 vec4_from_vec3(vec3 v3) {
	vec4 v4;
	v4.x = v3.x;
	v4.y = v3.y;
	v4.z = v3.z;
	v4.w = 1.0;
	return v4;
}

vec3 vec3_from_vec4(vec4 v4) {
	vec3 v3;
	v3.x = v4.x;
	v3.y = v4.y;
	v3.z = v4.z;
	return v3;
}
