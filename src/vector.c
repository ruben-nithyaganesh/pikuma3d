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

mat3 mat3_add(mat3 m1, mat3 m2) {
	mat3 res;
	res.e00 = m1.e00 + m2.e00;
	res.e01 = m1.e01 + m2.e01;
	res.e02 = m1.e02 + m2.e02;

	res.e10 = m1.e10 + m2.e10;
	res.e11 = m1.e11 + m2.e11;
	res.e12 = m1.e12 + m2.e12;

	res.e20 = m1.e20 + m2.e20;
	res.e21 = m1.e21 + m2.e21;
	res.e22 = m1.e22 + m2.e22;

	return res;
}

mat3 mat3_sub(mat3 m1, mat3 m2) {
	mat3 res;
	res.e00 = m1.e00 - m2.e00;
	res.e01 = m1.e01 - m2.e01;
	res.e02 = m1.e02 - m2.e02;

	res.e10 = m1.e10 - m2.e10;
	res.e11 = m1.e11 - m2.e11;
	res.e12 = m1.e12 - m2.e12;

	res.e20 = m1.e20 - m2.e20;
	res.e21 = m1.e21 - m2.e21;
	res.e22 = m1.e22 - m2.e22;

	return res;
}

mat3 mat3_mul(mat3 m1, mat3 m2) {
	mat3 res;
	res.e00 = (m1.e00 * m2.e00) + (m1.e01 * m2.e10) + (m1.e02 * m2.e20);
	res.e01 = (m1.e00 * m2.e01) + (m1.e01 * m2.e11) + (m1.e02 * m2.e21);
	res.e02 = (m1.e00 * m2.e02) + (m1.e01 * m2.e12) + (m1.e02 * m2.e22);

	res.e10 = (m1.e10 * m2.e00) + (m1.e11 * m2.e10) + (m1.e12 * m2.e20);
	res.e11 = (m1.e10 * m2.e01) + (m1.e11 * m2.e11) + (m1.e12 * m2.e21);
	res.e12 = (m1.e10 * m2.e02) + (m1.e11 * m2.e12) + (m1.e12 * m2.e22);

	res.e20 = (m1.e20 * m2.e00) + (m1.e21 * m2.e10) + (m1.e22 * m2.e20);
	res.e21 = (m1.e20 * m2.e01) + (m1.e21 * m2.e11) + (m1.e22 * m2.e21);
	res.e22 = (m1.e20 * m2.e02) + (m1.e21 * m2.e12) + (m1.e22 * m2.e22);
	return res;
}

char* mat4_tostring(mat4 *m) {
	char* str = (char *)malloc(sizeof(char) * 1000);
	sprintf(str,\
	"[%f, %f, %f, %f]\n\
[%f, %f, %f, %f]\n\
[%f, %f, %f, %f]\n\
[%f, %f, %f, %f]",\
	m->e00, m->e01, m->e02, m->e03,\
	m->e10, m->e11, m->e12, m->e13,\
	m->e20, m->e21, m->e22, m->e23,\
	m->e30, m->e31, m->e32, m->e33);
	return str;
}

void mat4_load_from_text(const char* filename, mat4 *m) {
	FILE *file = fopen(filename, "r");
	fscanf(file,\
	"%f %f %f %f\n\
%f %f %f %f\n\
%f %f %f %f\n\
%f %f %f %f",\
	&m->e00, &m->e01, &m->e02, &m->e03,\
	&m->e10, &m->e11, &m->e12, &m->e13,\
	&m->e20, &m->e21, &m->e22, &m->e23,\
	&m->e30, &m->e31, &m->e32, &m->e33);
	fclose(file);
}


mat4 mat4_mul(mat4 m1, mat4 m2) {
	mat4 result;
	result.e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20 + m1.e03 * m2.e30;
	result.e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21 + m1.e03 * m2.e31;
	result.e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22 + m1.e03 * m2.e32;
	result.e03 = m1.e00 * m2.e03 + m1.e01 * m2.e13 + m1.e02 * m2.e23 + m1.e03 * m2.e33;

	result.e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20 + m1.e13 * m2.e30;
	result.e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21 + m1.e13 * m2.e31;
	result.e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22 + m1.e13 * m2.e32;
	result.e13 = m1.e10 * m2.e03 + m1.e11 * m2.e13 + m1.e12 * m2.e23 + m1.e13 * m2.e33;

	result.e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20 + m1.e23 * m2.e30; 
	result.e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21 + m1.e23 * m2.e31;
	result.e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22 + m1.e23 * m2.e32;
	result.e23 = m1.e20 * m2.e03 + m1.e21 * m2.e13 + m1.e22 * m2.e23 + m1.e23 * m2.e33;

	result.e30 = m1.e30 * m2.e00 + m1.e31 * m2.e10 + m1.e32 * m2.e20 + m1.e33 * m2.e30; 
	result.e31 = m1.e30 * m2.e01 + m1.e31 * m2.e11 + m1.e32 * m2.e21 + m1.e33 * m2.e31;
	result.e32 = m1.e30 * m2.e02 + m1.e31 * m2.e12 + m1.e32 * m2.e22 + m1.e33 * m2.e32;
	result.e33 = m1.e30 * m2.e03 + m1.e31 * m2.e13 + m1.e32 * m2.e23 + m1.e33 * m2.e33;
	return result;
}
