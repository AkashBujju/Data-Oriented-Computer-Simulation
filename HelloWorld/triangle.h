#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math.h"

struct Triangle {
	unsigned int vao;
	unsigned int vbo;
	int program;
	Matrix4 model;
};
typedef struct Triangle Triangle;

void make_triangle(Triangle *triangle, int program);
void draw_triangle(Triangle *triangle, const Matrix4* view, const Matrix4* projection);
void delete_triangle(Triangle *triangle);

#endif
