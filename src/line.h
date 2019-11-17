#ifndef LINE_H
#define LINE_H

#include "math.h"

struct Line {
	unsigned int vao;
	unsigned int vbo;
	int program;
	Matrix4 model;
	Vector3 color;
};
typedef struct Line Line;

void make_line(Line *line, Vector3* from, Vector3* to, int program);
void draw_line(Line *line, Matrix4* view, Matrix4* projection);

#endif
