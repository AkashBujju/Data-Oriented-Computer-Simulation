#include "math.h"

void init_matrix(Matrix4 *mat) {
	float *m = mat->matrix;
	m[0] = 0; m[1] = 0; m[2] = 0; m[3] = 0;
	m[4] = 0; m[5] = 0; m[6] = 0; m[7] = 0;
	m[8] = 0; m[9] = 0; m[10] = 0; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 0;
}

void make_identity(Matrix4 *mat) {
	float *m = mat->matrix;
	m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0;
	m[4] = 0; m[5] = 1; m[6] = 0; m[7] = 0;
	m[8] = 0; m[9] = 0; m[10] = 1; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}
