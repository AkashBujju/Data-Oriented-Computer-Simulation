#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math.h"

struct Triangle {
	unsigned int vao;
	unsigned int vbo;
	unsigned int program;
	Matrix4 model;
};
typedef struct Triangle Triangle;

void make_triangle(Triangle *triangle);
void draw_triangle(Triangle *triangle, Matrix4* view, Matrix4* projection);
void delete_triangle(Triangle *triangle);

#endif
