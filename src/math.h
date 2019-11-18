#ifndef MY_MATH_H
#define MY_MATH_H

struct Vector3 {
	float x;
	float y;
	float z;
};
typedef struct Vector3 Vector3;

struct Vector4 {
	float x;
	float y;
	float z;
	float a;
};
typedef struct Vector4 Vector4;

struct Vector {
	Vector3 point;
	Vector3 direction;
};
typedef struct Vector Vector;

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
float to_radians(float degree);
void init_vector(Vector3 *vec, float x, float y, float z);
void init_vector4(Vector4 *vec, float x, float y, float z, float a);
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
Vector4 sub4(Vector4* vec1, Vector4* vec2);
Vector4 add4(Vector4* vec1, Vector4* vec2);
Vector3 scalar_mul(Vector3* vec, float val);
float matrix_determinant(Matrix4* mat);
Matrix4 matrix_adjoint(Matrix4 *mat);
Matrix4 matrix_inverse(Matrix4 *mat);
Vector3 normalize_to(float x, float y, int width, int height);
Vector4 mul_vec_and_mat4(Vector4 *vec, Matrix4 *mat);
Vector4 mul_mat4_and_vec(Matrix4 *mat, Vector4 *vec);
Vector compute_mouse_ray(float norm_x, float norm_y, Matrix4 *view, Matrix4 *projection);

#endif
