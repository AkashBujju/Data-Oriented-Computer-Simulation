#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math.h"

struct Triangle {
	unsigned int vao;
	unsigned int vbo;
	unsigned int program;
	Vector3 rotation_axes;
	Vector3 color;
	float angle_in_degree;
	Matrix4 model;
};
typedef struct Triangle Triangle;

void make_triangle(Triangle *triangle);
void draw_triangle(Triangle *triangle, Matrix4* view, Matrix4* projection);
void rotate_triangle(Triangle* triangle, Vector3* axes, float angle_in_degree);
void delete_triangle(Triangle *triangle);

#endif
