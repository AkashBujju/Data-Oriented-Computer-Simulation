#ifndef MY_MATH_H
#define MY_MATH_H

/* Eqn of Line in vector form */
struct LineEq {
	float x, y, z;
	float a, b, c;
};
typedef struct LineEq LineEq;

struct Vector2 {
	float x;
	float y;
};
typedef struct Vector2 Vector2;

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

struct Box {
	Vector3 top_left;
	Vector3 top_right;
	Vector3 bottom_left;
	Vector3 bottom_right;
	Vector3 center;
};
typedef struct Box Box;

void init_matrix(Matrix4 *mat);
void make_identity(Matrix4 *mat);
void scale(Matrix4 *mat, float x, float y, float z);
float to_radians(float degree);
void init_vector(Vector3 *vec, float x, float y, float z);
void init_vector2(Vector2 *vec, float x, float y);
void init_vector4(Vector4 *vec, float x, float y, float z, float a);
void copy_vector(Vector3 *to, Vector3 *from);
void translateBy_vector(Vector3 *vec, float x, float y, float z);
void translate_vector(Vector3 *vec, float x, float y, float z);
void translate_matrix(Matrix4 *mat, float x, float y, float z);
void translateBy_matrix(Matrix4 *mat, float x, float y, float z);
void rotate_z(Matrix4 *mat, float theta);
void rotate_y(Matrix4 *mat, float theta);
void rotate_x(Matrix4 *mat, float theta);
Vector3 rotate_point(Vector3* p, Vector3* axes, float degree);
void rotate(Matrix4 *mat, Vector3* axes, float degree);
void rotate_about(Matrix4 *mat, Vector3* axes, Vector3* about, float degree);
void multiply_matrix(Matrix4 *m1, Matrix4 *m2);
Matrix4 perspective(const float fov, const float aspect_ratio, const float z_near, const float z_far);
Matrix4 ortho(float left, float right, float bottom, float top);
void print_matrix(Matrix4 *mat);
void print_vector(Vector3 *vec);
void print_vector2(Vector2 *vec);
void print_line(LineEq *line_eq);
void copy_matrix(Matrix4 *from, Matrix4 *to);
void normalize_vector(Vector3 *vec);
Matrix4 look_at(Vector3* position, Vector3* target, Vector3* up);
Vector3 cross(Vector3 *vec1, Vector3 *vec2);
float dot(Vector3 *vec1, Vector3 *vec2);
Vector3 sub(Vector3* vec1, Vector3* vec2);
Vector3 add(Vector3* vec1, Vector3* vec2);
Vector4 sub4(Vector4* vec1, Vector4* vec2);
Vector4 add4(Vector4* vec1, Vector4* vec2);
Vector3 scalar_mul(Vector3* vec, float val);
float f_min(float f1, float f2);
float f_max(float f1, float f2);
float matrix_determinant(Matrix4* mat);
Matrix4 matrix_adjoint(Matrix4 *mat);
Matrix4 matrix_inverse(Matrix4 *mat);
Vector3 normalize_to(float x, float y, int width, int height);
float f_normalize(float value, float from_range_start, float from_range_end, float to_range_start, float to_range_end);
Vector4 mul_vec_and_mat4(Vector4 *vec, Matrix4 *mat);
Vector4 mul_mat4_and_vec(Matrix4 *mat, Vector4 *vec);
Vector compute_mouse_ray(float norm_x, float norm_y, Matrix4 *view, Matrix4 *projection);
Vector compute_mouse_ray_2(float norm_x, float norm_y, Matrix4 *view, Matrix4 *projection);
int in_plane_point(Box *box, Vector3 *res, Vector3 *ray_start, Vector3* ray_end);
float get_distance(Vector3 *p1, Vector3 *p2);
LineEq form_line(Vector3 *p1, Vector3 *p2);
Vector3 line_intersection(LineEq *l1, LineEq *l2);
LineEq form_line(Vector3 *p1, Vector3 *p2);
Vector3 line_intersect(LineEq *line_eq_1, LineEq *line_eq_2);

#endif
