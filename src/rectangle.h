#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "math.h"

struct Rectangle {
	unsigned int vao;
	unsigned int vbo;
	unsigned int texture_id;
	unsigned int program;
	Matrix4 model;
	Vector3 position;
	Vector3 scale;
	Vector3 rotation_axes;
	float angle_in_degree;
	float width, height;
};
typedef struct Rectangle Rectangle;

void make_rectangle(Rectangle *rectangle, int program, const char* image);
void translate_rectangle(Rectangle *rectangle, float x, float y, float z);
void rotate_rectangle(Rectangle *rectangle, float x, float y, float z, float degree);
void scale_rectangle(Rectangle *rectangle, float x, float y, float z);
void draw_rectangle(Rectangle *rectangle, const Matrix4* view, const Matrix4* projection);
int i_quad(Rectangle *rectangle, Vector *ray, Vector3* result);
void delete_rectangle(Rectangle *rectangle);

#endif
