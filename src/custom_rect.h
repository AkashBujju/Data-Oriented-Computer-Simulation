#ifndef CUSTOM_RECT
#define CUSTOM_RECT

#include "math.h"

struct CustomRect {
	unsigned int vao;
	unsigned int vbo;
	unsigned int texture_id;
	unsigned int program;
	Vector3 ll, ul, ur, lr;
	Vector2 t_ll, t_ul, t_ur, t_lr;
	Matrix4 model;
	Vector3 position;
};
typedef struct CustomRect CustomRect;

void make_custom_rect(CustomRect *custom_rect, Vector3 *p1, Vector3 *p2, Vector3 *p3, Vector3 *p4, int program, int texture_id, Vector2 *t_p1, Vector2 *t_p2, Vector2 *t_p3, Vector2 *t_p4);
void draw_custom_rect(CustomRect *custom_rect, const Matrix4* view, const Matrix4* projection);
void translate_custom_rect(CustomRect *custom_rect, float x, float y, float z);
void delete_custom_rect(CustomRect *custom_rect);
int i_custom_rects(CustomRect *custom_rects, int count, Vector *ray, Vector3* result, Vector3* filter_axes);
int i_custom_rect(CustomRect *custom_rect, Vector *ray, Vector3* result);

#endif
