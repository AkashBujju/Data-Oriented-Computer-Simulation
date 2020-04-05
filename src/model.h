#ifndef MODEL_H
#define MODEL_H

#include "math.h"

struct Model {
	unsigned int vao;
	unsigned int vbo;
	unsigned int texture_id;
	unsigned int program;
	unsigned int num_opengl_vertices;
	Matrix4 model;
	Vector3 position;
	Vector3 local_origin;
	Vector3 scale;
	Vector3 rotation_axes;
	float angle_in_degree;
	float width, height, depth;
};
typedef struct Model Model;

void make_model(Model *model, int program, float* vertices, int float_count, int num_opengl_vertices, const char* image_path, Vector3* local_origin, float width, float height, float depth);
void draw_model(Model *model, const Matrix4* view, const Matrix4* projection);
void scale_model(Model *model, float x, float y, float z);
void translate_model(Model *model, float x, float y, float z);
void rotate_model(Model *model, float x, float y, float z, float degree);

#endif
