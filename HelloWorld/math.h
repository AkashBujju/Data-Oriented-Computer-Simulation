#ifndef MY_MATH_H
#define MY_MATH_H

struct Vector3 {
	float x;
	float y;
	float z;
};
typedef struct Vector3 Vector3;

struct Matrix4 {
	float matrix[16];
};
typedef struct Matrix4 Matrix4;

void init_matrix(Matrix4 *mat);
void make_identity(Matrix4 *mat);

#endif
