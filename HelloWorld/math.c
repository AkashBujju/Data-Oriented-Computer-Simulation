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

void scale(Matrix4 *mat, float x, float y, float z) {
	float *m = mat->matrix;
	if(x != 0) {
		m[0] *= x;
		m[4] *= x;
	}
	if(y != 0) {
		m[1] *= y;
		m[5] *= y;
	}

	// @Incomplete: if(z != 0)?
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

Matrix4 rotate_z(Matrix4 *mat, float degree) {
	float in_radians = degree * 0.0174533f;
	Matrix4 tmp;
	make_identity(&tmp);

	float *m1 = mat->matrix;
	float *m2 = tmp.matrix;

	m2[0] = cos(in_radians);
	m2[1] = sin(in_radians);
	m2[4] = -m2[1];
	m2[5] = m2[0];

	return multiply_matrix(mat, &tmp);
}

Matrix4 rotate_y(Matrix4 *mat, float degree) {
	float in_radians = degree * 0.0174533f;
	Matrix4 tmp;
	make_identity(&tmp);

	float *m1 = mat->matrix;
	float *m2 = tmp.matrix;

	m2[0] = cos(in_radians);
	m2[8] = sin(in_radians);
	m2[2] = -m2[8];
	m2[10] = m2[0];

	return multiply_matrix(mat, &tmp);
}

Matrix4 rotate_x(Matrix4 *mat, float degree) {
	float in_radians = degree * 0.0174533f;
	Matrix4 tmp;
	make_identity(&tmp);

	float *m1 = mat->matrix;
	float *m2 = tmp.matrix;

	m2[5] = cos(in_radians);
	m2[6] = sin(in_radians);
	m2[9] = -m2[6];
	m2[10] = m2[5];

	return multiply_matrix(mat, &tmp);
}

Matrix4 multiply_matrix(Matrix4 *m1, Matrix4 *m2) {
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
	
	return res;
}

Matrix4 perspective(const float fov, const float aspect_ratio, const float z_near, const float z_far) {
	Matrix4 res;
	init_matrix(&res);
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

void print_matrix(Matrix4 *mat) {
	const float *ptr = mat->matrix;
	for(int i = 0; i < 16; ++i) {
		if(i % 4 == 0)
			printf("\n");
		printf("%.2f ", ptr[i]);
	}
	printf("\n\n");

}
