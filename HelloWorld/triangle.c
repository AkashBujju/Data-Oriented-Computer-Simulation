#include <glad/glad.h>
#include <stdio.h> // @Tmp
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
}

void draw_triangle(Triangle *triangle, unsigned int program) {
	glUseProgram(program);

	make_identity(&triangle->model);

	triangle->model = rotate_z(&triangle->model, 30);
	scale(&triangle->model, 0.25f, 0.25, 0);
	translate_matrix(&triangle->model, 0.5f, 0.5f, 0);

	set_matrix4(program, "transform", &triangle->model);

	glBindVertexArray(triangle->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void delete_triangle(Triangle *triangle) {
	glDeleteVertexArrays(1, &triangle->vao);
	glDeleteBuffers(1, &triangle->vbo);
}
