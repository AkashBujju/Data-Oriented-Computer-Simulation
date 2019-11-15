#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "rect.h"
#include "shader.h"
#include "utils.h"

void make_rect(Rect *rect, int program, const char *image) {
	rect->program = program;
	float vertices[60];
	read_floats_from_file("data\\rect_vertices.dat", vertices);

	glGenVertexArrays(1, &rect->vao);
	glGenBuffers(1, &rect->vbo);
	glBindVertexArray(rect->vao);

	glBindBuffer(GL_ARRAY_BUFFER, rect->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	rect->texture_id = make_texture(image);
	init_matrix(&rect->model);
}

void draw_rect(Rect *rect, const Matrix4 *view, const Matrix4* projection) {
	glUseProgram(rect->program);

	float time = (float)glfwGetTime();
	float degrees = sin(time) * 30;

	make_identity(&rect->model);
	Vector3 axes;
	init_vector(&axes, 0, 0, 1);

	Vector3 point;
	init_vector(&point, 0, 2, -1);
	scale(&rect->model, 0.5f, 2.0f, 1);
	rotate_about(&rect->model, &axes, &point, degrees);
	set_matrix4(rect->program, "model", &rect->model);

	glBindTexture(GL_TEXTURE_2D, rect->texture_id);
	glBindVertexArray(rect->vao);
	glDrawArrays(GL_TRIANGLES, 0, 12);
}
