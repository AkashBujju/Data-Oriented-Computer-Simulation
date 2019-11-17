#include <glad/glad.h>
#include "shader.h"
#include "rectangle.h"
#include "utils.h"

void make_rectangle(Rectangle *rectangle, int program, const char* image) {
	rectangle->program = program;

	float vertices[30];
	read_floats_from_file("..\\data\\rectangle_vertices.dat", vertices);

	glGenVertexArrays(1, &rectangle->vao);
	glGenBuffers(1, &rectangle->vbo);
	glBindVertexArray(rectangle->vao);

	glBindBuffer(GL_ARRAY_BUFFER, rectangle->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	rectangle->texture_id = make_texture(image);
	init_matrix(&rectangle->model);
	init_vector(&rectangle->position, 0, 0, 0);
	init_vector(&rectangle->scale, 1, 1, 1);
	init_vector(&rectangle->rotation_axes, 0, 0, 0);
}

void translate_rectangle(Rectangle *rectangle, float x, float y, float z) {
	rectangle->position.x = x;
	rectangle->position.y = y;
	rectangle->position.z = z;
}

void scale_rectangle(Rectangle *rectangle, float x, float y, float z) {
	rectangle->scale.x = x;
	rectangle->scale.y = y;
	rectangle->scale.z = z;
}

void rotate_rectangle(Rectangle *rectangle, float x, float y, float z, float degree) {
	rectangle->angle_in_degree = degree;
	rectangle->rotation_axes.x = x;
	rectangle->rotation_axes.y = y;
	rectangle->rotation_axes.z = z;
}

void draw_rectangle(Rectangle *rectangle, const Matrix4* view, const Matrix4* projection) {
	glUseProgram(rectangle->program);

	make_identity(&rectangle->model);
	translate_matrix(&rectangle->model, rectangle->position.x, rectangle->position.y, rectangle->position.z);
	scale(&rectangle->model, rectangle->scale.x, rectangle->scale.y, rectangle->scale.z);
	rotate(&rectangle->model, &rectangle->rotation_axes, rectangle->angle_in_degree);
	set_matrix4(rectangle->program, "model", &rectangle->model);

	glBindTexture(GL_TEXTURE_2D, rectangle->texture_id);
	glBindVertexArray(rectangle->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void delete_rectangle(Rectangle *rectangle) {
	glDeleteVertexArrays(1, &rectangle->vao);
	glDeleteBuffers(1, &rectangle->vbo);
}
