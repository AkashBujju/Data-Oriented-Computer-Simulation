#include <glad/glad.h>
#include "grid.h"
#include "shader.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h> // @Tmp

void make_grid(Grid *grid, int num_rows, int num_cols, float per_width, float per_height) {
	grid->num_rows = num_rows;
	grid->num_cols = num_cols;
	grid->per_width = per_width;
	grid->per_height = per_height;

	const float grid_width = num_rows * per_width;
	const float grid_height = num_cols * per_height;
	grid->num_vertices = ((num_rows + 1) * 2 + (num_cols + 1) * 2);
	const unsigned int num_floats =  grid->num_vertices * 3;
	float* vertices = (float*)malloc(sizeof(float) * num_floats);

	// filling rows
	unsigned int index = 0;
	Vector3 current_point;
	init_vector(&current_point, -grid_width / 2.0f, +grid_height / 2.0f, 0);
	for(int i = 0; i < num_rows + 1; ++i) {
		Vector3 p1, p2;
		init_vector(&p1, current_point.x, current_point.y, current_point.z);
		init_vector(&p2, current_point.x + grid_width, current_point.y, current_point.z);

		vertices[index++] = p1.x;
		vertices[index++] = p1.y;
		vertices[index++] = p1.z;
		vertices[index++] = p2.x;
		vertices[index++] = p2.y;
		vertices[index++] = p2.z;

		current_point.y -= per_height;
	}

	// filling cols
	init_vector(&current_point, -grid_width / 2.0f, -grid_height / 2.0f, 0);
	for(int i = 0; i < num_cols + 1; ++i) {
		Vector3 p1, p2;
		init_vector(&p1, current_point.x, current_point.y, current_point.z);
		init_vector(&p2, current_point.x, current_point.y + grid_height, current_point.z);

		vertices[index++] = p1.x;
		vertices[index++] = p1.y;
		vertices[index++] = p1.z;
		vertices[index++] = p2.x;
		vertices[index++] = p2.y;
		vertices[index++] = p2.z;

		current_point.x += per_width;
	}

	grid->program = compile_shader("..\\shaders\\v_shader.shader", "..\\shaders\\f_shader.shader");

	glGenVertexArrays(1, &grid->vao);
	glGenBuffers(1, &grid->vbo);
	glBindVertexArray(grid->vao);
	glBindBuffer(GL_ARRAY_BUFFER, grid->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * num_floats, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	init_matrix(&grid->model);
	init_vector(&grid->position, 0, 0, 0);
	init_vector(&grid->scale, 1, 1, 1);
	init_vector(&grid->rotation_axes, 0, 0, 0);
	init_vector(&grid->color, 1, 1, 1);

	const float width_by_2 = grid_width / 2;
	const float height_by_2 = grid_height / 2;
	init_vector(&grid->box.top_left, -width_by_2, height_by_2, 0);
	init_vector(&grid->box.top_right, width_by_2, height_by_2, 0);
	init_vector(&grid->box.bottom_left, -width_by_2, -height_by_2, 0);
	init_vector(&grid->box.bottom_right, width_by_2, -height_by_2, 0);
	free(vertices);
}

void translate_grid(Grid* grid, float x, float y, float z) {
	grid->position.x = x;
	grid->position.y = y;
	grid->position.z = z;
	translate_box(&grid->box, x, y, z);
}

void rotate_grid(Grid* grid, float x, float y, float z, float degree) {
	grid->angle_in_degree = degree;
	grid->rotation_axes.x = x;
	grid->rotation_axes.y = y;
	grid->rotation_axes.z = z;
	rotate_box(&grid->box, &grid->rotation_axes, degree);
}

void translate_box(Box* box, float x, float y, float z) {
	float width_by_2 = (box->top_right.x - box->top_left.x) / 2;
	float height_by_2 = (box->top_right.y - box->bottom_right.y) / 2;

	init_vector(&box->center, x, y, z);
	init_vector(&box->top_left, x - width_by_2, y + height_by_2, z);
	init_vector(&box->top_right, x + width_by_2, y + height_by_2, z);
	init_vector(&box->bottom_left, x - width_by_2, y - height_by_2, z);
	init_vector(&box->bottom_right, x + width_by_2, y - height_by_2, z);
}

void rotate_box(Box *box, Vector3 *axes, float degree) {
	box->center= rotate_point(&box->center, axes, degree);
	box->top_left = rotate_point(&box->top_left, axes, degree);
	box->top_right = rotate_point(&box->top_right, axes, degree);
	box->bottom_right = rotate_point(&box->bottom_right, axes, degree);
	box->bottom_left = rotate_point(&box->bottom_left, axes, degree);
}

int get_sub_grid_mid_point(Grid *grid, Vector3* p, Vector3 *res) {
	// @Note: Here we check the 'z' co-ordinate for height, because the grid is rotated on the x-axis.
	// This won't work if the grid is in some other orientation.

	float p_width = p->x - grid->box.top_left.x;
	float actual_width = grid->per_width * grid->num_cols;
	if(p_width < 0 || p_width > actual_width)
		return 0;

	float x_ratio = p_width / actual_width;
	unsigned int x_index = ceil(x_ratio * grid->num_cols) - 1;

	float p_height = p->z - grid->box.bottom_left.z;
	float actual_height = grid->per_height * grid->num_rows;
	if(p_height < 0 || p_height > actual_height)
		return 0;

	float z_ratio = p_height / actual_height;
	unsigned int z_index = ceil(z_ratio * grid->num_rows) - 1;

	res->y = grid->box.center.y;
	res->x = grid->box.top_left.x + (x_index * grid->per_width) + (grid->per_width / 2);
	res->z = grid->box.top_left.z - (z_index * grid->per_height) - (grid->per_height / 2);

	return 1;
}

// void scale_grid(Grid* grid, float x, float y, float z) {
// 	grid->scale.x = x;
// 	grid->scale.y = y;
// 	grid->scale.z = z;
// }

void draw_grid(Grid *grid, Matrix4* view, Matrix4* projection) {
	glUseProgram(grid->program);

	make_identity(&grid->model);
	translate_matrix(&grid->model, grid->position.x, grid->position.y, grid->position.z);
	scale(&grid->model, grid->scale.x, grid->scale.y, grid->scale.z);
	rotate(&grid->model, &grid->rotation_axes, grid->angle_in_degree);
	set_vector3(grid->program, "aColor", &grid->color);
	set_matrix4(grid->program, "model", &grid->model);
	set_matrix4(grid->program, "view", view);
	set_matrix4(grid->program, "projection", projection);

	glBindVertexArray(grid->vao);
	glDrawArrays(GL_LINES, 0, grid->num_vertices);
}
