#ifndef RECT_H
#define RECT_H

#include "math.h"

struct Rect {
	unsigned int vao;
	unsigned int vbo;
	int texture_id;
	int program;
	Matrix4 model;
};
typedef struct Rect Rect;

void make_rect(Rect *rect, int program, const char *image);
void draw_rect(Rect *rect, const Matrix4 *view, const Matrix4* projection);

#endif
