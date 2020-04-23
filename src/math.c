#include "math.h"
#include <math.h>
#include <string.h>
#include <stdio.h> // @Tmp

float f_max(float f1, float f2) {
	if (f1 > f2)
		return f1;

	return f2;
}

float f_min(float f1, float f2) {
	if (f1 < f2)
		return f1;

	return f2;
}

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

void init_vector2(Vector2 *vec, float x, float y) {
	vec->x = x;
	vec->y = y;
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

Matrix4 ortho(float left, float right, float bottom, float top) {
	Matrix4 res;
	make_identity(&res);
	float *m = res.matrix;
	m[0] = 2 / (right - left);
	m[5] = 2 / (top - bottom);
	m[10] = -1;
	m[12] = -(right + left) / (right - left);
	m[13] = -(top + bottom) / (top - bottom);

	return res;
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

float f_normalize(float value, float from_range_start, float from_range_end, float to_range_start, float to_range_end) {
	float old_diff = from_range_start - from_range_end;
	float new_diff = to_range_start - to_range_end;
	float new_value = (((value - from_range_start) * new_diff) / old_diff) + to_range_start;

	return new_value;
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

Vector compute_mouse_ray_2(float norm_x, float norm_y, Matrix4 *view, Matrix4 *projection) {
	Matrix4 mat_world;
	Matrix4 inv_view = matrix_inverse(view);
	Matrix4 inv_projection = matrix_inverse(projection);
	copy_matrix(&inv_projection, &mat_world);
	multiply_matrix(&mat_world, &inv_view);

	Vector4 near, far;
	init_vector4(&near, norm_x, norm_y, -1, 1);
	init_vector4(&far, norm_x, norm_y, 1, 1);
	near = mul_vec_and_mat4(&near, &mat_world);
	far = mul_vec_and_mat4(&far, &mat_world);

	near.x /= near.a;
	near.y /= near.a;
	near.z /= near.a;

	far.x /= far.a;
	far.y /= far.a;
	far.z /= far.a;

	Vector vec;
	Vector3 to;
	init_vector(&vec.point, near.x, near.y, near.z);
	init_vector(&to, far.x, far.y, far.z);

	vec.direction = sub(&to, &vec.point);
	normalize_vector(&vec.direction);

	return vec;
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

int in_plane_point(Box *box, Vector3 *res, Vector3 *ray_start, Vector3* ray_end) {
	Vector3 vec_1 = sub(&box->top_right, &box->top_left);
	Vector3 vec_2 = sub(&box->bottom_left, &box->top_left);

	Vector3 plane_normal = cross(&vec_1, &vec_2);
	normalize_vector(&plane_normal);

	Vector3 ray_delta = sub(ray_end, ray_start);
	Vector3 ray_to_plane_delta = sub(&box->center, ray_start);

	float wp = dot(&ray_to_plane_delta, &plane_normal);
	float vp = dot(&ray_delta, &plane_normal);
	float k = wp / vp;

	copy_vector(res, &ray_delta);
	*res = scalar_mul(res, k);
	*res = add(res, ray_start);

	if(k >= 0)
		return 1;
	
	return 0;
}

float get_distance(Vector3 *p1, Vector3 *p2) {
	float inner = (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y) + (p1->z - p2->z) * (p1->z - p2->z);
	if(inner <= 0.001f)
		return 0;

	return sqrt(inner);
}

LineEq form_line(Vector3 *p1, Vector3 *p2) {
	LineEq line_eq;
	line_eq.x = p1->x;
	line_eq.y = p1->y;
	line_eq.z = p1->z;
	line_eq.a = p2->x - p1->x;
	line_eq.b = p2->y - p1->y;
	line_eq.c = p2->z - p1->z;

	return line_eq;
}

Vector3 line_intersect(LineEq *line_eq_1, LineEq *line_eq_2) {
	Vector3 result;
	init_vector(&result, 0, 0, 0);

	float lambda, u;
	float lambda_1, n1, u_1;
	float lambda_2, n2, u_2;
	float to_check_dim_1, to_check_const_1;
	float to_check_dim_2, to_check_const_2;

	/* eqn_2: n1 + lambda = u */
	/* eqn_2: n2 + lambda = u */

	if(line_eq_1->a == 0 && line_eq_2->a == 0) {
		lambda_1 = line_eq_1->b; u_1 = line_eq_2->b; n1 = line_eq_1->y - line_eq_2->y;
		lambda_2 = line_eq_1->c; u_2 = line_eq_2->c; n2 = line_eq_1->z - line_eq_2->z;
		to_check_dim_1 = line_eq_1->x; to_check_const_1 = line_eq_1->a;
		to_check_dim_2 = line_eq_2->x; to_check_const_2 = line_eq_2->a;
	}
	else if(line_eq_1->b == 0 && line_eq_2->b == 0) {
		lambda_1 = line_eq_1->a; u_1 = line_eq_2->a; n1 = line_eq_1->x - line_eq_2->x;
		lambda_2 = line_eq_1->c; u_2 = line_eq_2->c; n2 = line_eq_1->z - line_eq_2->z;
		to_check_dim_1 = line_eq_1->y; to_check_const_1 = line_eq_1->b;
		to_check_dim_2 = line_eq_2->y; to_check_const_2 = line_eq_2->b;
	}
	else {
		lambda_1 = line_eq_1->a; u_1 = line_eq_2->a; n1 = line_eq_1->x - line_eq_2->x;
		lambda_2 = line_eq_1->b; u_2 = line_eq_2->b; n2 = line_eq_1->y - line_eq_2->y;
		to_check_dim_1 = line_eq_1->z; to_check_const_1 = line_eq_1->c;
		to_check_dim_2 = line_eq_2->z; to_check_const_2 = line_eq_2->c;
	}

	if(fabs(u_1) <= 0.0001f) {
		lambda = -n1 / lambda_1;
	}
	else {
		lambda_1 /= u_1;
		n1 /= u_1;
		u_1 = 1;
	}

	if(fabs(lambda_2) <= 0.0001f) {
		u = n2 / u_2;
	}
	else if(fabs(u_2) <= 0.0001f) {
		lambda = -n2 / lambda_2;
		u = n1 + lambda_1 * lambda;
	}
	else {
		lambda_2 /= u_2;
		n2 /= u_2;
		u_2 = 1;
		lambda = (n1 - n2) / (lambda_2 - lambda_1);
		u = n1 + lambda_1 * lambda;
	}

	/* Checking if lambda and u satisfy eqn_3 */
	float lhs = to_check_dim_1 + lambda * to_check_const_1;
	float rhs = to_check_dim_2 + u * to_check_const_2;
	if(fabs(lhs - rhs) <= 0.0001f) {
		result.x = line_eq_1->x + lambda * line_eq_1->a;
		result.y = line_eq_1->y + lambda * line_eq_1->b;
		result.z = line_eq_1->z + lambda * line_eq_1->c;
	}
	else {
		/* They do not intersect */
		printf("Lines do not intersect!\n");
	}

	return result;
}

void print_line(LineEq *line_eq) {
	printf("Vector form: a: %.2f, b: %.2f, c: %.2f :: %.2f %.2f %.2f\n", line_eq->a, line_eq->b, line_eq->c, line_eq->x, line_eq->y, line_eq->z);
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

void print_vector2(Vector2 *vec) {
	printf("%.2f %.2f\n", vec->x, vec->y);
}

void print_vector(Vector3 *vec) {
	printf("%.2f %.2f %.2f\n", vec->x, vec->y, vec->z);
}
