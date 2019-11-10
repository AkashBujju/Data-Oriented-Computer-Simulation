#include <glad/glad.h>
#include <stdio.h> // @Tmp
#include <GLFW/glfw3.h>
#include <math.h>
#include "triangle.h"
#include "shader.h"

void make_triangle(Triangle *triangle) {
	float vertices[] = {
		-1.0f, -1.0f, +0.0f, // left  
		+1.0f, -1.0f, +0.0f, // right 
		+0.0f, +1.0f, +0.0f  // top   
	};

	glGenVertexArrays(1, &triangle->vao);
	glGenBuffers(1, &triangle->vbo);
	glBindVertexArray(triangle->vao);

	glBindBuffer(GL_ARRAY_BUFFER, triangle->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	init_matrix(&triangle->model);
	init_vector(&triangle->pos, 0, 0, 0);

	// Matrix4 proj = perspective(45.0f, 1440 / 900.0f, 0.1f, 100.0f);
	// print_matrix(&proj);
}

void draw_triangle(Triangle *triangle, unsigned int program) {
	glUseProgram(program);

	float time = (float)glfwGetTime();
	static float degrees = 0;
	degrees = sin(time) * 360;

	make_identity(&triangle->model);

	Matrix4 model, view, projection;
	make_identity(&model);
	make_identity(&view);
	make_identity(&projection);

	translate_matrix(&view, 0, 0, -10.0f);
	projection = perspective(45.0f, 1440 / 900.0f, 0.1f, 100.0f);

	// triangle->model = rotate_z(&triangle->model, degrees);
	// scale(&triangle->model, 0.25f, 0.25, 0);
	// translate_matrix(&triangle->model, 0.5f, 0.5f, 0);

	set_matrix4(program, "model", &model);
	set_matrix4(program, "view", &view);
	set_matrix4(program, "projection", &projection);

	glBindVertexArray(triangle->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void delete_triangle(Triangle *triangle) {
	glDeleteVertexArrays(1, &triangle->vao);
	glDeleteBuffers(1, &triangle->vbo);
}
