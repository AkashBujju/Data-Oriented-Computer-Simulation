#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "cube.h"
#include "shader.h"
#include "utils.h"

void make_cuboid(Cuboid *cuboid, int program, const char* image) {
	cuboid->program = program;

	float vertices[180];
	read_floats_from_file("data\\cube_vertices.dat", vertices);

	glGenVertexArrays(1, &cuboid->vao);
	glGenBuffers(1, &cuboid->vbo);
	glBindVertexArray(cuboid->vao);

	glBindBuffer(GL_ARRAY_BUFFER, cuboid->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	cuboid->texture_id = make_texture(image);
	init_matrix(&cuboid->model);
}

void draw_cuboid(Cuboid *cuboid, const Matrix4* view, const Matrix4* projection) {
	glUseProgram(cuboid->program);

	float time = (float)glfwGetTime();
	static float degrees = 0;
	degrees = sin(time * 0.2f) * 360;

	make_identity(&cuboid->model);
	rotate_z(&cuboid->model, degrees);
	rotate_x(&cuboid->model, degrees);
	// scale(&cuboid->model, 2.0f, 0, 0);
	set_matrix4(cuboid->program, "model", &cuboid->model);

	glBindTexture(GL_TEXTURE_2D, cuboid->texture_id);
	glBindVertexArray(cuboid->vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void delete_cuboid(Cuboid *cuboid) {
	glDeleteVertexArrays(1, &cuboid->vao);
	glDeleteBuffers(1, &cuboid->vbo);
}
