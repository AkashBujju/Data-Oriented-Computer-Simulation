#ifndef CUBE_H
#define CUBE_H

#include "math.h"

struct Cuboid {
	unsigned int vao;
	unsigned int vbo;
	unsigned int texture_id;
	unsigned int program;
	Matrix4 model;
	Vector3 position;
	Vector3 scale;
	Vector3 rotation_axes;
	float angle_in_degree;
	float width, height, depth;
};
typedef struct Cuboid Cuboid;

void make_cuboid(Cuboid *cuboid, int program, const char* image);
void draw_cuboid(Cuboid *cuboid, const Matrix4* view, const Matrix4* projection);
void translate_cuboid(Cuboid *cuboid, float x, float y, float z);
void rotate_cuboid(Cuboid* cuboid, float x, float y, float z, float degree);
void scale_cuboid(Cuboid* cuboid, float x, float y, float z);
int test_aabb(Cuboid *cuboid, Vector *ray);
int test_obb(Cuboid *cuboid, Vector *ray);
void delete_cuboid(Cuboid *cuboid);

#endif
