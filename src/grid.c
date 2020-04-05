#include <glad/glad.h>
#include "grid.h"
#include "shader.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h> // @Tmp

extern const char* shaders_path;
extern const char* assets_path;
extern char* combine_string(const char*, const char*);
extern int shader1;

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

	grid->program = compile_shader(combine_string(shaders_path, "v_shader.shader"), combine_string(shaders_path, "f_shader.shader"));

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
	init_vector(&grid->color, 0.3f, 0.3f, 0.3f);

	const float width_by_2 = grid_width / 2;
	const float height_by_2 = grid_height / 2;
	init_vector(&grid->box.top_left, -width_by_2, height_by_2, 0);
	init_vector(&grid->box.top_right, width_by_2, height_by_2, 0);
	init_vector(&grid->box.bottom_left, -width_by_2, -height_by_2, 0);
	init_vector(&grid->box.bottom_right, width_by_2, -height_by_2, 0);
	free(vertices);

	/* Making rectangle */
	make_rectangle(&grid->background, shader1, combine_string(assets_path, "png/gray.png"));
	/* Making rectangle */
}

void translate_grid(Grid* grid, float x, float y, float z) {
	grid->position.x = x;
	grid->position.y = y;
	grid->position.z = z;
	translate_box(&grid->box, x, y, z);
}

void translate_grid_by(Grid* grid, float x, float y, float z) {
	translateBy_vector(&grid->position, x, y, z);
	translateBy_vector(&grid->box.center, x, y, z);
	translateBy_vector(&grid->box.top_left, x, y, z);
	translateBy_vector(&grid->box.top_right, x, y, z);
	translateBy_vector(&grid->box.bottom_left, x, y, z);
	translateBy_vector(&grid->box.bottom_right, x, y, z);
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
	box->center = rotate_point(&box->center, axes, degree);
	box->top_left = rotate_point(&box->top_left, axes, degree);
	box->top_right = rotate_point(&box->top_right, axes, degree);
	box->bottom_right = rotate_point(&box->bottom_right, axes, degree);
	box->bottom_left = rotate_point(&box->bottom_left, axes, degree);
}

int get_sub_grid_mid_point(Grid *grid, Vector3* p, Vector3 *res) {
	// @Note: Here we check the 'y' co-ordinate for height, because the grid is rotated on the x-axis.
	// This won't work if the grid is in some other orientation.

	// float p_width = p->x - grid->box.top_left.x;
	// float actual_width = grid->per_width * grid->num_cols;
	// if(p_width < 0 || p_width > actual_width)
	// 	return 0;

	// float x_ratio = p_width / actual_width;
	// unsigned int x_index = ceil(x_ratio * grid->num_cols) - 1;

	// float p_height = p->y - grid->box.bottom_left.y;
	// float actual_height = grid->per_height * grid->num_rows;
	// if(p_height < 0 || p_height > actual_height)
	// 	return 0;

	// float y_ratio = p_height / actual_height;
	// unsigned int y_index = ceil(y_ratio * grid->num_rows) - 1;

	// res->z = grid->box.center.z;
	// res->x = grid->box.top_left.x + (x_index * grid->per_width) + (grid->per_width / 2);
	// res->y = (y_index * grid->per_height) + (grid->per_height / 2) - grid->box.top_left.y;
	
	/* Width Test */
	Vector3 v_len = sub(&grid->box.top_right, &grid->box.top_left);
	Vector3 p_len = sub(p, &grid->box.top_left);
	float x_percent = dot(&p_len, &v_len) / dot(&v_len, &v_len);
	float per_percent = 1 / (float)grid->num_cols;
	x_percent = (floor(x_percent / per_percent) * per_percent) + (per_percent / 2);
	float x_distance = x_percent * grid->per_width * grid->num_rows;
	if(x_percent >= 1.0f)
		return 0;

	v_len = sub(&grid->box.bottom_left, &grid->box.top_left);
	p_len = sub(p, &grid->box.top_left);
	float y_percent = dot(&p_len, &v_len) / dot(&v_len, &v_len);
	per_percent = 1 / (float)grid->num_rows;
	y_percent = (floor(y_percent / per_percent) * per_percent) + (per_percent / 2);
	float y_distance = y_percent * grid->per_height * grid->num_cols;
	if(y_percent >= 1.0f)
		return 0;

	Vector3 dir1 = sub(&grid->box.top_right, &grid->box.top_left);
	normalize_vector(&dir1);
	Vector3 end_1 = scalar_mul(&dir1, x_distance);
	end_1 = add(&end_1, &grid->box.top_left);

	Vector3 dir2 = sub(&grid->box.bottom_left, &grid->box.top_left);
	normalize_vector(&dir2);
	Vector3 end_2 = scalar_mul(&dir2, y_distance);
	end_2 = add(&end_2, &end_1);

	copy_vector(res, &end_2);

	return 1;
}

void draw_grid(Grid *grid, Matrix4* view, Matrix4* projection) {
	glUseProgram(grid->program);

	make_identity(&grid->model);
	translate_matrix(&grid->model, grid->position.x, grid->position.y, grid->position.z);
	scale(&grid->model, grid->scale.x, grid->scale.y, grid->scale.z);

	/* Rotation */
	Vector3 tmp;
	init_vector(&tmp, grid->position.x, grid->position.y, grid->position.z);
	translate_matrix(&grid->model, 0, 0, 0);
	rotate(&grid->model, &grid->rotation_axes, grid->angle_in_degree);
	translate_matrix(&grid->model, tmp.x, tmp.y, tmp.z);
	/* Rotation */

	set_vector3(grid->program, "aColor", &grid->color);
	set_matrix4(grid->program, "model", &grid->model);
	set_matrix4(grid->program, "view", view);
	set_matrix4(grid->program, "projection", projection);

	glBindVertexArray(grid->vao);
	glDrawArrays(GL_LINES, 0, grid->num_vertices);

	draw_rectangle(&grid->background, view, projection);
}
