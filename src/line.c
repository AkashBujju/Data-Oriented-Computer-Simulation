#include "line.h"
#include <glad/glad.h>
#include <stdlib.h>
#include "shader.h"

void make_line(Line *line, Vector3* from, Vector3* to, int program) {
	line->program = program;
	float *vertices = (float*)malloc(sizeof(float) * 6);
	vertices[0] = from->x; vertices[1] = from->y; vertices[2] = from->z;
	vertices[3] = to->x; vertices[4] = to->y; vertices[5] = to->z;

	glGenVertexArrays(1, &line->vao);
	glGenBuffers(1, &line->vbo);
	glBindVertexArray(line->vao);
	glBindBuffer(GL_ARRAY_BUFFER, line->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices) * 6, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	init_matrix(&line->model);
	init_vector(&line->color, 1, 1, 1);

	free(vertices);
}

void draw_line(Line *line, Matrix4* view, Matrix4* projection) {
	glUseProgram(line->program);

	make_identity(&line->model);
	set_matrix4(line->program, "model", &line->model);
	set_matrix4(line->program, "view", view);
	set_matrix4(line->program, "projection", projection);
	set_vector3(line->program, "aColor", &line->color);

	glBindVertexArray(line->vao);
	glDrawArrays(GL_LINES, 0, 6);
}
