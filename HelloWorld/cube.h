#ifndef CUBE_H
#define CUBE_H

#include "math.h"

struct Cuboid {
	unsigned int vao;
	unsigned int vbo;
	int texture_id;
	int program;
	Matrix4 model;
};
typedef struct Cuboid Cuboid;

void make_cuboid(Cuboid *cuboid, int program, const char* image);
void draw_cuboid(Cuboid *cuboid, const Matrix4* view, const Matrix4* projection);
void delete_cuboid(Cuboid *cuboid);

#endif
