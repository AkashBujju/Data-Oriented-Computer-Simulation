#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math.h"

struct Triangle {
	unsigned int vao;
	unsigned int vbo;
	Matrix4 model;
	Vector3 pos;
};
typedef struct Triangle Triangle;

void make_triangle(Triangle *triangle);
void draw_triangle(Triangle *triangle, unsigned int program);
void delete_triangle(Triangle *triangle);

#endif
