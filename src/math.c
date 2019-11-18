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

void init_vector4(Vector4 *vec, float x, float y, float z, float a) {
	vec->x = x;
	vec->y = y;
	vec->z = z;
	vec->a = a;
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

float to_radians(float degree) {
	return degree * 0.0174533f;
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

Vector3 rotate_point(Vector3* p, Vector3* axes, float degree) {
	Vector3 res;
	Qt qr, qa;

	qa.a = 0;
	qa.x = p->x;
	qa.y = p->y;
	qa.z = p->z;

	float in_radians = degree * 0.0174533f;
	float by_2 = in_radians / 2;
	float _a = cos(by_2);
	float _x = axes->x * sin(by_2);
	float _y = axes->y * sin(by_2);
	float _z = axes->z * sin(by_2);

	qr.a = _a;
	qr.x = _x;
	qr.y = _y;
	qr.z = _z;

	res.x = qa.x * (qr.a * qr.a + qr.x * qr.x - qr.y * qr.y - qr.z * qr.z) + 2 * qa.y * (qr.x * qr.y - qr.a * qr.z) + 2 * qa.z * (qr.a * qr.y + qr.x * qr.z);

	res.y = 2 * qa.x * (qr.a * qr.z + qr.x * qr.y) + qa.y * (qr.a * qr.a - qr.x * qr.x + qr.y * qr.y - qr.z * qr.z) + 2 * qa.z * (qr.y * qr.z - qr.a * qr.x);

	res.z = 2 * qa.x * (qr.x * qr.z - qr.a * qr.y) + 2 * qa.y * (qr.a * qr.x + qr.y * qr.z) + qa.z * (qr.a * qr.a - qr.x * qr.x - qr.y * qr.y + qr.z * qr.z);

	return res;
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

Vector4 sub4(Vector4* vec1, Vector4* vec2) {
	Vector4 res;
	init_vector4(&res, vec1->x - vec2->x, vec1->y - vec2->y, vec1->z - vec2->z, vec1->a - vec2->a);
	
	return res;
}

Vector4 add4(Vector4* vec1, Vector4* vec2) {
	Vector4 res;
	init_vector4(&res, vec1->x + vec2->x, vec1->y + vec2->y, vec1->z + vec2->z, vec1->a + vec2->a);

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

float matrix_determinant(Matrix4* mat) {   
	float det = 0;
	float *m = mat->matrix;
	float a = m[0];
	float b = m[1];
	float c = m[2];
	float d = m[3];

	float d1 = +m[0] * (m[5] * m[10] * m[15] + m[6] * m[11] * m[13] + m[7] * m[9] * m[14] - m[7] * m[10] * m[13] - m[6] * m[9] * m[15] - m[5] * m[11] * m[14]);
	float d2 = -m[4] * (m[1] * m[10] * m[15] + m[2] * m[11] * m[13] + m[3] * m[9] * m[14] - m[3] * m[10] * m[13] - m[2] * m[9] * m[15] - m[1] * m[11] * m[14]);
	float d3 = +m[8] * (m[1] * m[6] * m[15] + m[2] * m[7] * m[13] + m[3] * m[5] * m[14] - m[3] * m[6] * m[13] - m[2] * m[5] * m[15] - m[1] * m[7] * m[14]);
	float d4 = -m[12] * (m[1] * m[6] * m[11] + m[2] * m[7] * m[9] + m[3] * m[5] * m[10] - m[3] * m[6] * m[9] - m[2] * m[5] * m[11] - m[1] * m[7] * m[10]);

	det = d1 + d2 + d3 + d4;

	return det;
}

Matrix4 matrix_adjoint(Matrix4 *mat) {
	Matrix4 res;
	float *m = mat->matrix;
	float *r = res.matrix;

	r[0] = (m[5] * m[10] * m[15]) + (m[6] * m[11] * m[13]) + (m[7] * m[9] * m[14]) - (m[7] * m[10] * m[13]) - (m[6] * m[9] * m[15]) - (m[5] * m[11] * m[14]);
	r[1] = -(m[1] * m[10] * m[15]) - (m[2] * m[11] * m[13]) - (m[3] * m[9] * m[14]) + (m[3] * m[10] * m[13]) + (m[2] * m[9] * m[15]) + (m[1] * m[11] * m[14]);
	r[2] = (m[1] * m[6] * m[15]) + (m[2] * m[7] * m[13]) + (m[3] * m[5] * m[14]) - (m[3] * m[6] * m[13]) - (m[2] * m[5] * m[15]) - (m[1] * m[7] * m[14]);
	r[3] = -(m[1] * m[6] * m[11]) - (m[2] * m[7] * m[9]) - (m[3] * m[5] * m[10]) + (m[3] * m[6] * m[9]) + (m[2] * m[5] * m[11]) + (m[1] * m[7] * m[10]);
	r[4] = -(m[4] * m[10] * m[15]) - (m[6] * m[11] * m[12]) - (m[7] * m[8] * m[14]) + (m[7] * m[10] * m[12]) + (m[6] * m[8] * m[15]) + (m[4] * m[11] * m[14]); 
	r[5] = (m[0] * m[10] * m[15]) + (m[2] * m[11] * m[12]) + (m[3] * m[8] * m[14]) - (m[3] * m[10] * m[12]) - (m[2] * m[8] * m[15]) - (m[0] * m[11] * m[14]);
	r[6] = -(m[0] * m[6] * m[15]) - (m[2] * m[7] * m[12]) - (m[3] * m[4] * m[14]) + (m[3] * m[6] * m[12]) + (m[2] * m[4] * m[15]) + (m[0] * m[7] * m[14]);
	r[7] = (m[0] * m[6] * m[11]) + (m[2] * m[7] * m[8]) + (m[3] * m[4] * m[10]) - (m[3] * m[6] * m[8]) - (m[2] * m[4] * m[11]) - (m[0] * m[7] * m[10]);
	r[8] = (m[4] * m[9] * m[15]) + (m[5] * m[11] * m[12]) + (m[7] * m[8] * m[13]) - (m[7] * m[9] * m[12]) - (m[5] * m[8] * m[15]) - (m[4] * m[11] * m[13]);
	r[9] = -(m[0] * m[9] * m[15]) - (m[1] * m[11] * m[12]) - (m[3] * m[8] * m[13]) + (m[3] * m[9] * m[12]) + (m[1] * m[8] * m[15]) + (m[0] * m[11] * m[13]);
	r[10] = (m[0] * m[5] * m[15]) + (m[1] * m[7] * m[12]) + (m[3] * m[4] * m[13]) - (m[3] * m[5] * m[12]) - (m[1] * m[4] * m[15]) - (m[0] * m[7] * m[13]); 
	r[11] = -(m[0] * m[5] * m[11]) - (m[1] * m[7] * m[8]) - (m[3] * m[4] * m[9]) + (m[3] * m[5] * m[8]) + (m[1] * m[4] * m[11]) + (m[0] * m[7] * m[9]);
	r[12] = -(m[4] * m[9] * m[14]) - (m[5] * m[10] * m[12]) - (m[6] * m[8] * m[13]) + (m[6] * m[9] * m[12]) + (m[5] * m[8] * m[14]) + (m[4] * m[10] * m[13]);
	r[13] = (m[0] * m[9] * m[14]) + (m[1] * m[10] * m[12]) + (m[2] * m[8] * m[13]) - (m[2] * m[9] * m[12]) - (m[1] * m[8] * m[14]) - (m[0] * m[10] * m[13]);
	r[14] = -(m[0] * m[5] * m[14]) - (m[1] * m[6] * m[12]) - (m[2] * m[4] * m[13]) + (m[2] * m[5] * m[12]) + (m[1] * m[4] * m[14]) + (m[0] * m[6] * m[13]);
	r[15] = (m[0] * m[5] * m[10]) + (m[1] * m[6] * m[8]) + (m[2] * m[4] * m[9]) - (m[2] * m[5] * m[8]) - (m[1] * m[4] * m[10]) - (m[0] * m[6] * m[9]);

	return res;
}

Matrix4 matrix_inverse(Matrix4 *mat) {
	float det = matrix_determinant(mat);
	Matrix4 inv, adj;
	if(det == 0) {
		printf("Matrix is singular, ie. det == 0.\n");
		return *mat;
	}

	adj = matrix_adjoint(mat);
	float *in = inv.matrix;
	float *ad = adj.matrix;

	for(int i = 0; i < 16; ++i)
		in[i] = ad[i] / det;

	return inv;
}

Vector3 normalize_to(float x, float y, int width, int height) {
	Vector3 res;
	init_vector(&res, x / width * 2 - 1, 1 - y / height * 2, 0);

	return res;
}

Vector4 mul_vec_and_mat4(Vector4 *vec, Matrix4 *mat) {
	Vector4 res;
	float *m = mat->matrix;
	float x = vec->x, y = vec->y, z = vec->z, w = vec->a;

	res.x = x * m[0] + y * m[4] + z * m[8] + w * m[12];
	res.y = x * m[1] + y * m[5] + z * m[9] + w * m[13];
	res.z = x * m[2] + y * m[6] + z * m[10] + w * m[14];
	res.a = x * m[3] + y * m[7] + z * m[11] + w * m[15];

	return res;
}

Vector4 mul_mat4_and_vec(Matrix4 *mat, Vector4 *vec) {
	Vector4 res;
	float *m = mat->matrix;
	float x = vec->x, y = vec->y, z = vec->z, w = vec->a;

	res.x = x * m[0] + y * m[1] + z * m[2] + w * m[3];
	res.y = x * m[4] + y * m[5] + z * m[6] + w * m[7];
	res.z = x * m[8] + y * m[9] + z * m[10] + w * m[11];
	res.a = x * m[12] + y * m[13] + z * m[14] + w * m[15];

	return res;
}

Vector compute_mouse_ray(float norm_x, float norm_y, Matrix4 *view, Matrix4 *projection) {
	Vector4 clip;
	init_vector4(&clip, norm_x, norm_y, -1, 1);

	Matrix4 invProj = matrix_inverse(projection);
	Vector4 eye = mul_vec_and_mat4(&clip, &invProj);
	eye.z = -1;
	eye.a = 0;

	Matrix4 invView = matrix_inverse(view);
	Vector4 world = mul_vec_and_mat4(&eye, &invView);
	Vector3 ray;
	init_vector(&ray, world.x, world.y, world.z);
	normalize_vector(&ray);

	Vector res;
	init_vector(&res.direction, ray.x, ray.y, ray.z);
	init_vector(&res.point, invView.matrix[12], invView.matrix[13], invView.matrix[14]);

	return res;
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
