#ifndef GRID_H
#define GRID_H

#include "math.h"

struct Grid {
	unsigned int vao;
	unsigned int vbo;
	int texture_id;
	int program;
	unsigned int num_vertices;
	int num_rows, num_cols;
	float per_width, per_height;
	Matrix4 model;
	Vector3 position;
	Vector3 scale;
	Vector3 rotation_axes;
	Vector3 color;
	float angle_in_degree;
	Box box;
};
typedef struct Grid Grid;

void make_grid(Grid *grid, int num_rows, int num_cols, float per_width, float per_height);
void draw_grid(Grid *grid, Matrix4* view, Matrix4* projection);
void translate_grid(Grid* grid, float x, float y, float z);
void rotate_grid(Grid* grid, float x, float y, float z, float degree);
void translate_box(Box* box, float x, float y, float z);
void rotate_box(Box *box, Vector3 *axes, float degree);
// void scale_grid(Grid* grid, float x, float y, float z);

#endif
