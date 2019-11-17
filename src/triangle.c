#include <glad/glad.h>
#include "triangle.h"
#include "shader.h"

void make_triangle(Triangle *triangle) {
	triangle->program = compile_shader("..\\shaders\\v_shader.shader", "..\\shaders\\f_shader.shader");

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
}

void draw_triangle(Triangle *triangle, Matrix4* view, Matrix4* projection) {
	glUseProgram(triangle->program);

	make_identity(&triangle->model);
	set_matrix4(triangle->program, "model", &triangle->model);
	set_matrix4(triangle->program, "view", view);
	set_matrix4(triangle->program, "projection", projection);

	glBindVertexArray(triangle->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void delete_triangle(Triangle *triangle) {
	glDeleteVertexArrays(1, &triangle->vao);
	glDeleteBuffers(1, &triangle->vbo);
}
