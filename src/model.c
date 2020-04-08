#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h> // @TMP
#include "shader.h"
#include "utils.h"
#include "model.h"

extern char* combine_string(const char* str_1, const char* str_2);
extern const char* assets_path;

void make_model(Model *model, int program, int texture_id, float* vertices, int float_count, int num_opengl_vertices, Vector3* local_origin, float width, float height, float depth) {
	model->program = program;
	model->num_opengl_vertices = num_opengl_vertices;

	glGenVertexArrays(1, &model->vao);
	glGenBuffers(1, &model->vbo);
	glBindVertexArray(model->vao);
	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * float_count, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	model->texture_id = texture_id;
	model->angle_in_degree = 0;
	model->width = width;
	model->height = height;
	model->depth = depth;
	init_matrix(&model->model);
	init_vector(&model->position, 0, 0, 0);
	copy_vector(&model->local_origin, local_origin);
	init_vector(&model->scale, 1, 1, 1);
	init_vector(&model->rotation_axes, 0, 0, 0);
}

void translate_model(Model *model, float x, float y, float z) {
	model->position.x = x;
	model->position.y = y;
	model->position.z = z;
}

void rotate_model(Model *model, float x, float y, float z, float degree) {
	model->angle_in_degree = degree;
	model->rotation_axes.x = x;
	model->rotation_axes.y = y;
	model->rotation_axes.z = z;
}

void scale_model(Model *model, float x, float y, float z) {
	model->scale.x = x;
	model->scale.y = y;
	model->scale.z = z;

	model->width *= x;
	model->height *= y;
	model->depth *= z;
}

void draw_model(Model *model, const Matrix4* view, const Matrix4* projection) {
	glUseProgram(model->program);
	make_identity(&model->model);

	/* Translation & Scaling */
	translate_matrix(&model->model, model->position.x, model->position.y, model->position.z);
	scale(&model->model, model->scale.x, model->scale.y, model->scale.z);
	/* Translation & Scaling */

	/* Rotation */
	Vector3 tmp;
	init_vector(&tmp, model->position.x, model->position.y, model->position.z);
	translate_matrix(&model->model, 0, 0, 0);
	rotate(&model->model, &model->rotation_axes, model->angle_in_degree);
	translate_matrix(&model->model, tmp.x, tmp.y, tmp.z);
	set_matrix4(model->program, "model", &model->model);
	/* Rotation */

	glBindTexture(GL_TEXTURE_2D, model->texture_id);
	glBindVertexArray(model->vao);
	glDrawArrays(GL_TRIANGLES, 0, model->num_opengl_vertices);
}
