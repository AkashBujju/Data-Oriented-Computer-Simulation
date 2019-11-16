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
	init_vector(&cuboid_uv->pos, 0, 0, 0);
}

void translate_cuboid_uv(CuboidUV *cuboid_uv, float x, float y, float z) {
	cuboid_uv->pos.x = x;
	cuboid_uv->pos.y = y;
	cuboid_uv->pos.z = z;
}

void draw_cuboid_uv(CuboidUV *cuboid_uv, const Matrix4 *view, const Matrix4* projection) {
	glUseProgram(cuboid_uv->program);

	float time = (float)glfwGetTime();
	float degrees = sin(time * 0.5f) * 360;

	make_identity(&cuboid_uv->model);
	Vector3 axes;
	init_vector(&axes, 1, 1, 0);

	Vector3 point;
	init_vector(&point, 0, 2, -1);
	// scale(&cuboid_uv->model, 0.5f, 2.0f, 1);

	translate_matrix(&cuboid_uv->model, cuboid_uv->pos.x, cuboid_uv->pos.y, cuboid_uv->pos.z);
	// rotate_about(&cuboid_uv->model, &axes, &point, degrees);
	// rotate(&cuboid_uv->model, &axes, degrees);
	set_matrix4(cuboid_uv->program, "model", &cuboid_uv->model);

	glBindTexture(GL_TEXTURE_2D, cuboid_uv->texture_id);
	glBindVertexArray(cuboid_uv->vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
