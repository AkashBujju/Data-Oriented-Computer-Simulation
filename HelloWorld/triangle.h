#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math.h"

struct Triangle {
	unsigned int vao;
	unsigned int vbo;
	Matrix4 model;
};
typedef struct Triangle Triangle;

void make_triangle(Triangle *triangle);
void draw_triangle(Triangle *triangle, unsigned int program);
void delete_triangle(Triangle *triangle);

#endif
