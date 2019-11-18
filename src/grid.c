#include <glad/glad.h>
#include "grid.h"
#include "shader.h"
#include <stdlib.h>

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

	free(vertices);
}

void translate_grid(Grid* grid, float x, float y, float z) {
	grid->position.x = x;
	grid->position.y = y;
	grid->position.z = z;
}

void rotate_grid(Grid* grid, float x, float y, float z, float degree) {
	grid->angle_in_degree = degree;
	grid->rotation_axes.x = x;
	grid->rotation_axes.y = y;
	grid->rotation_axes.z = z;
}

void scale_grid(Grid* grid, float x, float y, float z) {
	grid->scale.x = x;
	grid->scale.y = y;
	grid->scale.z = z;
}

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