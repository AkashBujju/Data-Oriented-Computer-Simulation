#ifndef RECT_H
#define RECT_H

#include "math.h"

struct CuboidUV {
	unsigned int vao;
	unsigned int vbo;
	int texture_id;
	int program;
	Matrix4 model;
	Vector3 position;
	Vector3 scale;
	Vector3 rotation_axes;
	float angle_in_degree;
};
typedef struct CuboidUV CuboidUV;

void make_cuboid_uv(CuboidUV *cuboid_uv, int program, const char *image);
void draw_cuboid_uv(CuboidUV *cuboid_uv, const Matrix4 *view, const Matrix4* projection);
void translate_cuboid_uv(CuboidUV *cuboid_uv, float x, float y, float z);
void rotate_cuboid_uv(CuboidUV* cuboid_uv, float x, float y, float z, float degree);
void scale_cuboid_uv(CuboidUV* cuboid_uv, float x, float y, float z);

#endif
