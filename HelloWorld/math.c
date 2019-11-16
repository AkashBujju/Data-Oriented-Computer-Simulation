#include "math.h"
#include <math.h>
#include <string.h>
#include <stdio.h> // @Tmp

void init_matrix(Matrix4 *mat) {
	float *m = mat->matrix;
	memset(m, 0, sizeof(float) * 16);
}

void make_identity(Matrix4 *mat) {
	float *m = mat->matrix;
	memset(m, 0, sizeof(float) * 16);
	m[0] = 1; m[5] = 1; m[10] = 1; m[15] = 1;
}

void init_vector(Vector3 *vec, float x, float y, float z) {
	vec->x = x;
	vec->y = y;
	vec->z = z;
}

void copy_vector(Vector3 *to, Vector3 *from) {
	to->x = from->x;
	to->y = from->y;
	to->z = from->z;
}

void normalize_vector(Vector3 *vec) {
	const float n = 1.0f / sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
	vec->x *= n;
	vec->y *= n;
	vec->z *= n;
}

void scale(Matrix4 *mat, float x, float y, float z) {
	float *m = mat->matrix;
	m[0] *= x; m[1] *= x; m[2] *= x;
	m[4] *= y; m[5] *= y; m[6] *= y;
	m[8] *= z; m[9] *= z; m[10] *= z;
}

void translateBy_vector(Vector3 *vec, float x, float y, float z) {
	vec->x += x;
	vec->y += y;
	vec->z += z;
}

void translate_vector(Vector3 *vec, float x, float y, float z) {
	vec->x = x;
	vec->y = y;
	vec->z = z;
}

void translate_matrix(Matrix4 *mat, float x, float y, float z) {
	float *m = mat->matrix;
	m[12] = x;
	m[13] = y;
	m[14] = z;
}

void translateBy_matrix(Matrix4 *mat, float x, float y, float z) {
	float *m = mat->matrix;
	m[12] += x;
	m[13] += y;
	m[14] += z;
}

void rotate_z(Matrix4 *mat, float degree) {
	float in_radians = degree * 0.0174533f;
	Matrix4 tmp;
	make_identity(&tmp);

	float *m1 = mat->matrix;
	float *m2 = tmp.matrix;

	m2[0] = cos(in_radians);
	m2[1] = sin(in_radians);
	m2[4] = -m2[1];
	m2[5] = m2[0];

	multiply_matrix(mat, &tmp);
}

void rotate_y(Matrix4 *mat, float degree) {
	float in_radians = degree * 0.0174533f;
	Matrix4 tmp;
	make_identity(&tmp);

	float *m1 = mat->matrix;
	float *m2 = tmp.matrix;

	m2[0] = cos(in_radians);
	m2[8] = sin(in_radians);
	m2[2] = -m2[8];
	m2[10] = m2[0];

	multiply_matrix(mat, &tmp);
}

void rotate_x(Matrix4 *mat, float degree) {
	float in_radians = degree * 0.0174533f;
	Matrix4 tmp;
	make_identity(&tmp);

	float *m1 = mat->matrix;
	float *m2 = tmp.matrix;

	m2[5] = cos(in_radians);
	m2[6] = sin(in_radians);
	m2[9] = -m2[6];
	m2[10] = m2[5];

	multiply_matrix(mat, &tmp);
}

void rotate(Matrix4 *mat, Vector3* axes, float degree) {
	float in_radians = degree * 0.0174533f;
	float by_2 = in_radians / 2;
	Qt q;
	q.a = cos(by_2);
	q.x =	axes->x * sin(by_2);
	q.y = axes->y * sin(by_2);
	q.z = axes->z * sin(by_2);

	// normalizing
	const float n = 1.0f / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.a * q.a);
	q.a *= n;
	q.x *= n;
	q.y *= n;
	q.z *= n;

	Matrix4 mat2;
	float *m = mat2.matrix;
	m[0] = 1 - (2 * q.y * q.y) - (2 * q.z * q.z);
	m[1] = (2 * q.x * q.y) - (2 * q.z * q.a);
	m[2] = 2 * q.x * q.z + 2 * q.y * q.a;
	m[3] = 0;
	m[4] = 2 * q.x * q.y + 2 * q.z * q.a;
	m[5] = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
	m[6] = 2 * q.y * q.z - 2 * q.x * q.a;
	m[7] = 0;
	m[8] = 2 * q.x * q.z - 2 * q.y * q.a;
	m[9] = 2 * q.y * q.z + 2 * q.x * q.a;
	m[10] = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
	m[11] = 0;
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;

	multiply_matrix(mat, &mat2);
}

void rotate_about(Matrix4 *mat, Vector3* axes, Vector3* about, float degree) {
	translateBy_matrix(mat, -about->x, -about->y, -about->z);
	rotate(mat, axes, degree);
	translateBy_matrix(mat, about->x, about->y, about->z);
}

Matrix4 look_at(Vector3* position, Vector3* target, Vector3* up) {
	Matrix4 res;
	make_identity(&res);
	float *r = res.matrix;

	Vector3 f, s, u;
	f = sub(target, position);
	normalize_vector(&f);
	s = cross(&f, up);
	normalize_vector(&s);
	u = cross(&s, &f);

	r[0] = s.x;
	r[4] = s.y;
	r[8] = s.z;
	r[1] = u.x;
	r[5] = u.y;
	r[9] = u.z;
	r[2] = -f.x;
	r[6] = -f.y;
	r[10] = -f.z;
	r[12] = -dot(&s, position);
	r[13] = -dot(&u, position);
	r[14] = dot(&f, position);

	return res;
}

Vector3 cross(Vector3 *vec1, Vector3 *vec2) {
	Vector3 res;
	res.x = vec1->y * vec2->z - vec1->z * vec2->y; 
	res.y = vec1->z * vec2->x - vec1->x * vec2->z; 
	res.z = vec1->x * vec2->y - vec1->y * vec2->x;

	return res;
}

float dot(Vector3 *vec1, Vector3 *vec2) {
	float res = 0;
	res += vec1->x * vec2->x;
	res += vec1->y * vec2->y;
	res += vec1->z * vec2->z;

	return res;
}

Vector3 sub(Vector3* vec1, Vector3* vec2) {
	Vector3 res;
	init_vector(&res, vec1->x - vec2->x, vec1->y - vec2->y, vec1->z - vec2->z);

	return res;
}

Vector3 add(Vector3* vec1, Vector3* vec2) {
	Vector3 res;
	init_vector(&res, vec1->x + vec2->x, vec1->y + vec2->y, vec1->z + vec2->z);

	return res;
}

Vector3 scalar_mul(Vector3* vec, float val) {
	Vector3 res;
	res.x = vec->x * val;
	res.y = vec->y * val;
	res.z = vec->z * val;

	return res;
}

void multiply_matrix(Matrix4 *m1, Matrix4 *m2) {
	Matrix4 res;
	init_matrix(&res);
	float *r = res.matrix;
	float *a = m1->matrix;
	float *b = m2->matrix;

	// r[i][j] += a[i][k] * b[k][j];	
	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			for(int k = 0; k < 4; ++k)
				r[4 * i + j] += a[4 * i + k] * b[4 * k + j];

	copy_matrix(&res, m1);
}

Matrix4 perspective(const float fov, const float aspect_ratio, const float z_near, const float z_far) {
	Matrix4 res;
	make_identity(&res);
	float *m = res.matrix;
	float fov_in_radians = 0.0174533f * fov;

	const float tan_half_fov = tan(fov_in_radians / 2.0f);
	m[0] = 1.0f / (aspect_ratio * tan_half_fov);
	m[5] = 1.0f / tan_half_fov;
	m[10] = -(z_far + z_near) / (z_far - z_near);
	m[11] = -1;
	m[14] = -(2 * z_far * z_near) / (z_far - z_near);

	return res;
}

inline void copy_matrix(Matrix4 *from, Matrix4 *to) {
	for(int i = 0; i < 16; ++i)
		to->matrix[i] = from->matrix[i];
}

void print_matrix(Matrix4 *mat) {
	const float *ptr = mat->matrix;
	for(int i = 0; i < 16; ++i) {
		if(i % 4 == 0)
			printf("\n");
		printf("%.2f ", ptr[i]);
	}
	printf("\n\n");
}

void print_vector(Vector3 *vec) {
	printf("%.2f %.2f %.2f\n", vec->x, vec->y, vec->z);
}
