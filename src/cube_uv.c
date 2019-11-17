#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "cube_uv.h"
#include "shader.h"
#include "utils.h"

void make_cuboid_uv(CuboidUV *cuboid_uv, int program, const char *image) {
	cuboid_uv->program = program;
	float vertices[180];
	read_floats_from_file("..\\data\\cuboid_uv_vertices.dat", vertices);

	glGenVertexArrays(1, &cuboid_uv->vao);
	glGenBuffers(1, &cuboid_uv->vbo);
	glBindVertexArray(cuboid_uv->vao);

	glBindBuffer(GL_ARRAY_BUFFER, cuboid_uv->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	cuboid_uv->texture_id = make_texture(image);
	init_matrix(&cuboid_uv->model);
	init_vector(&cuboid_uv->position, 0, 0, 0);
	init_vector(&cuboid_uv->scale, 1, 1, 1);
	init_vector(&cuboid_uv->rotation_axes, 0, 0, 0);
	cuboid_uv->angle_in_degree = 0;
}

void translate_cuboid_uv(CuboidUV *cuboid_uv, float x, float y, float z) {
	cuboid_uv->position.x = x;
	cuboid_uv->position.y = y;
	cuboid_uv->position.z = z;
}

void rotate_cuboid_uv(CuboidUV* cuboid_uv, float x, float y, float z, float degree) {
	cuboid_uv->angle_in_degree = degree;
	cuboid_uv->rotation_axes.x = x;
	cuboid_uv->rotation_axes.y = y;
	cuboid_uv->rotation_axes.z = z;
}

void scale_cuboid_uv(CuboidUV* cuboid_uv, float x, float y, float z) {
	cuboid_uv->scale.x = x;
	cuboid_uv->scale.y = y;
	cuboid_uv->scale.z = z;
}

void draw_cuboid_uv(CuboidUV *cuboid_uv, const Matrix4 *view, const Matrix4* projection) {
	glUseProgram(cuboid_uv->program);

	float time = (float)glfwGetTime();
	float degrees = sin(time * 0.5f) * 360;

	make_identity(&cuboid_uv->model);
	translate_matrix(&cuboid_uv->model, cuboid_uv->position.x, cuboid_uv->position.y, cuboid_uv->position.z);
	scale(&cuboid_uv->model, cuboid_uv->scale.x, cuboid_uv->scale.y, cuboid_uv->scale.z);
	rotate(&cuboid_uv->model, &cuboid_uv->rotation_axes, cuboid_uv->angle_in_degree);
	set_matrix4(cuboid_uv->program, "model", &cuboid_uv->model);

	glBindTexture(GL_TEXTURE_2D, cuboid_uv->texture_id);
	glBindVertexArray(cuboid_uv->vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
