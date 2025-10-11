#include "matrix.h"

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
	result.e00 = (m1.e00 * m2.e00) + (m1.e01 * m2.e10) + (m1.e02 * m2.e20) + (m1.e03 * m2.e30);
	result.e01 = (m1.e00 * m2.e01) + (m1.e01 * m2.e11) + (m1.e02 * m2.e21) + (m1.e03 * m2.e31);
	result.e02 = (m1.e00 * m2.e02) + (m1.e01 * m2.e12) + (m1.e02 * m2.e22) + (m1.e03 * m2.e32);
	result.e03 = (m1.e00 * m2.e03) + (m1.e01 * m2.e13) + (m1.e02 * m2.e23) + (m1.e03 * m2.e33);

	result.e10 = (m1.e10 * m2.e00) + (m1.e11 * m2.e10) + (m1.e12 * m2.e20) + (m1.e13 * m2.e30);
	result.e11 = (m1.e10 * m2.e01) + (m1.e11 * m2.e11) + (m1.e12 * m2.e21) + (m1.e13 * m2.e31);
	result.e12 = (m1.e10 * m2.e02) + (m1.e11 * m2.e12) + (m1.e12 * m2.e22) + (m1.e13 * m2.e32);
	result.e13 = (m1.e10 * m2.e03) + (m1.e11 * m2.e13) + (m1.e12 * m2.e23) + (m1.e13 * m2.e33);

	result.e20 = (m1.e20 * m2.e00) + (m1.e21 * m2.e10) + (m1.e22 * m2.e20) + (m1.e23 * m2.e30); 
	result.e21 = (m1.e20 * m2.e01) + (m1.e21 * m2.e11) + (m1.e22 * m2.e21) + (m1.e23 * m2.e31);
	result.e22 = (m1.e20 * m2.e02) + (m1.e21 * m2.e12) + (m1.e22 * m2.e22) + (m1.e23 * m2.e32);
	result.e23 = (m1.e20 * m2.e03) + (m1.e21 * m2.e13) + (m1.e22 * m2.e23) + (m1.e23 * m2.e33);

	result.e30 = (m1.e30 * m2.e00) + (m1.e31 * m2.e10) + (m1.e32 * m2.e20) + (m1.e33 * m2.e30); 
	result.e31 = (m1.e30 * m2.e01) + (m1.e31 * m2.e11) + (m1.e32 * m2.e21) + (m1.e33 * m2.e31);
	result.e32 = (m1.e30 * m2.e02) + (m1.e31 * m2.e12) + (m1.e32 * m2.e22) + (m1.e33 * m2.e32);
	result.e33 = (m1.e30 * m2.e03) + (m1.e31 * m2.e13) + (m1.e32 * m2.e23) + (m1.e33 * m2.e33);
	return result;
}

mat4 mat4_identity() {
	mat4 eye = {0};
	eye.e00 = 1.0;
	eye.e11 = 1.0;
	eye.e22 = 1.0;
	eye.e33 = 1.0;
	return eye;
}

mat4 mat4_scale_matrix(float sx, float sy, float sz) {
	// [sx 0  0  0]
	// [0  sy 0  0]
	// [0  0  sz 0]
	// [0  0  0  1]
	mat4 sm = {0};
	sm.e00 = sx;
	sm.e11 = sy;
	sm.e22 = sz;
	sm.e33 = 1.0;
	return sm;
}

mat4 mat4_translation_matrix(float tx, float ty, float tz) {
	// [1 0 0 tx]
	// [0 1 0 ty]
	// [0 0 1 tz]
	// [0 0 0 1 ]
	mat4 sm = mat4_identity();
	sm.e03 = tx;
	sm.e13 = ty;
	sm.e23 = tz;
	return sm;
}

mat4 mat4_rotation_matrix_x(float a) {
	// x rotation
	// [1 0 	 0 		 0]
	// [0 cos(a) -sin(a) 0]
	// [0 sin(a) cos(a)  0]
	// [0 0      0       1]
	mat4 rm_x = mat4_identity();
	rm_x.e11 = cos(a);
	rm_x.e12 = -sin(a);
	rm_x.e21 = sin(a);
	rm_x.e22 = cos(a);

	return rm_x;
}

mat4 mat4_rotation_matrix_y(float a) {
	// y rotation
	// [cos(a)  0  sin(a) 0]
	// [0       1  0      0]
	// [-sin(a) 0  cos(a) 0]
	// [0       0  0      1]
	mat4 rm_y = mat4_identity();
	rm_y.e00 = cos(a);
	rm_y.e02 = sin(a);
	rm_y.e20 = -sin(a);
	rm_y.e22 = cos(a);

	return rm_y;
}

mat4 mat4_rotation_matrix_z(float a) {
	// z rotation
	// [cos(a) -sin(a) 0 0]
	// [sin(a) cos(a)  0 0]
	// [0	   0       1 0]
	// [0      0       0 1]
	mat4 rm_z = mat4_identity();
	rm_z.e00 = cos(a);
	rm_z.e01 = -sin(a);
	rm_z.e10 = sin(a);
	rm_z.e11 = cos(a);
	
	return rm_z;
}

mat4 mat4_projection_matrix(float fov, float aspect, float znear, float zfar) {
	mat4 projection = {0};

	float tan_term = 1.0 / (tan(fov / 2.0));
	projection.e00 = aspect * tan_term;
	projection.e11 = tan_term;
	projection.e22 = zfar / (zfar - znear);
	projection.e23 = (-zfar * znear) / (zfar - znear);
	projection.e32 = 1.0;
	
	return projection;
}

vec4 mat4_project_and_normalise(vec4 v, mat4 proj) {
	vec4 res = mat4_mul_vec4(proj, v);
	if(res.w != 0) {
		res.x /= res.w;
		res.y /= res.w;
		res.z /= res.w;
	}
	return res;
}

vec4 mat4_mul_vec4(mat4 m, vec4 v) {
	vec4 res;
	res.x = v.x * m.e00 + v.y * m.e01 + v.z * m.e02 + v.w * m.e03;
	res.y = v.x * m.e10 + v.y * m.e11 + v.z * m.e12 + v.w * m.e13;
	res.z = v.x * m.e20 + v.y * m.e21 + v.z * m.e22 + v.w * m.e23;
	res.w = v.x * m.e30 + v.y * m.e31 + v.z * m.e32 + v.w * m.e33;
	return res;
}
