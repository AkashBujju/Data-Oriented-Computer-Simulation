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

// Quaternion
struct Qt {
	float a;
	float x, y, z;
};
typedef struct Qt Qt;

void init_matrix(Matrix4 *mat);
void make_identity(Matrix4 *mat);
void scale(Matrix4 *mat, float x, float y, float z);
void init_vector(Vector3 *vec, float x, float y, float z);
void copy_vector(Vector3 *to, Vector3 *from);
void translateBy_vector(Vector3 *vec, float x, float y, float z);
void translate_vector(Vector3 *vec, float x, float y, float z);
void translate_matrix(Matrix4 *mat, float x, float y, float z);
void translateBy_matrix(Matrix4 *mat, float x, float y, float z);
void rotate_z(Matrix4 *mat, float theta);
void rotate_y(Matrix4 *mat, float theta);
void rotate_x(Matrix4 *mat, float theta);
void multiply_matrix(Matrix4 *m1, Matrix4 *m2);
Matrix4 perspective(const float fov, const float aspect_ratio, const float z_near, const float z_far);
void print_matrix(Matrix4 *mat);
void print_vector(Vector3 *vec);
void copy_matrix(Matrix4 *from, Matrix4 *to);
void rotate(Matrix4 *mat, Vector3* axes, float degree);
void rotate_about(Matrix4 *mat, Vector3* axes, Vector3* about, float degree);
void normalize_vector(Vector3 *vec);
Matrix4 look_at(Vector3* position, Vector3* target, Vector3* up);
Vector3 cross(Vector3 *vec1, Vector3 *vec2);
float dot(Vector3 *vec1, Vector3 *vec2);
Vector3 sub(Vector3* vec1, Vector3* vec2);
Vector3 add(Vector3* vec1, Vector3* vec2);
Vector3 scalar_mul(Vector3* vec, float val);

#endif
