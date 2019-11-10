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
	m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0;
	m[4] = 0; m[5] = 1; m[6] = 0; m[7] = 0;
	m[8] = 0; m[9] = 0; m[10] = 1; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
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
	init_matrix(&tmp);
	make_identity(&tmp);

	float *m1 = mat->matrix;
	float *m2 = tmp.matrix;

	m2[0] = cos(in_radians);
	m2[1] = sin(in_radians);
	m2[4] = -sin(in_radians);
	m2[5] = cos(in_radians);

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

// void print_matrix(Matrix4 *mat) {
// 	const float *ptr = mat->matrix;
// 	for(int i = 0; i < 16; ++i) {
// 		if(i % 4 == 0)
// 			printf("\n");
// 		printf("%.2f ", ptr[i]);
// 	}
// 	printf("\n\n");
// 
// }
