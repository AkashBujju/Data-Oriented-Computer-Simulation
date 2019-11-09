#ifndef TRIANGLE_H
#define TRIANGLE_H

struct Triangle {
	unsigned int vao;
	unsigned int vbo;
};
#define Triangle struct Triangle

void make_triangle(Triangle *triangle);
void draw_triangle(Triangle *triangle, unsigned int program);
void delete_triangle(Triangle *triangle);

#endif
