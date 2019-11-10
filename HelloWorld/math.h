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
void scale(Matrix4 *mat, float x, float y, float z);
void init_vector(Vector3 *vec, float x, float y, float z);
void translateBy_vector(Vector3 *vec, float x, float y, float z);
void translate_vector(Vector3 *vec, float x, float y, float z);
void translate_matrix(Matrix4 *mat, float x, float y, float z);
Matrix4 rotate_z(Matrix4 *mat, float theta);
Matrix4 rotate_y(Matrix4 *mat, float theta);
Matrix4 rotate_x(Matrix4 *mat, float theta);
Matrix4 multiply_matrix(Matrix4 *m1, Matrix4 *m2);
Matrix4 perspective(const float fov, const float aspect_ratio, const float z_near, const float z_far);
void print_matrix(Matrix4 *mat);

#endif
