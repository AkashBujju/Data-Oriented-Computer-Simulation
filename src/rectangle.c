#include <glad/glad.h>
#include <stdlib.h>
#include "shader.h"
#include "rectangle.h"
#include "utils.h"

extern const char* assets_path;
extern char* combine_string(const char*, const char*);

void make_rectangle(Rectangle *rectangle, int program, const char* image) {
	rectangle->program = program;

	float *vertices = (float*)malloc(sizeof(float) * 30);
	read_floats_from_file(combine_string(assets_path, "vertices/rectangle_vertices.dat"), vertices);

	glGenVertexArrays(1, &rectangle->vao);
	glGenBuffers(1, &rectangle->vbo);
	glBindVertexArray(rectangle->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rectangle->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 30, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	rectangle->texture_id = make_texture(image);
	rectangle->angle_in_degree = 0;
	rectangle->width = 2;
	rectangle->height = 2;
	init_matrix(&rectangle->model);
	init_vector(&rectangle->position, 0, 0, 0);
	init_vector(&rectangle->scale, 1, 1, 1);
	init_vector(&rectangle->rotation_axes, 0, 0, 0);

	free(vertices);
}

void translate_rectangle(Rectangle *rectangle, float x, float y, float z) {
	rectangle->position.x = x;
	rectangle->position.y = y;
	rectangle->position.z = z;
}

void scale_rectangle(Rectangle *rectangle, float x, float y, float z) {
	rectangle->scale.x = x;
	rectangle->scale.y = y;
	rectangle->scale.z = z;

	rectangle->width *= x;
	rectangle->height *= y;
}

void rotate_rectangle(Rectangle *rectangle, float x, float y, float z, float degree) {
	rectangle->angle_in_degree = degree;
	rectangle->rotation_axes.x = x;
	rectangle->rotation_axes.y = y;
	rectangle->rotation_axes.z = z;
}

int i_quad(Rectangle *rectangle, Vector *ray, Vector3* result) {
	Vector3 plane_normal;
	float *matrix = rectangle->model.matrix;
	init_vector(&plane_normal, matrix[8], matrix[9], matrix[10]);
	normalize_vector(&plane_normal);

	Vector3 ray_end;
	ray_end = scalar_mul(&ray->direction, 200);
	ray_end = add(&ray->point, &ray_end);

	Vector3 ray_delta = sub(&ray_end, &ray->point);
	Vector3 ray_to_plane_delta = sub(&rectangle->position, &ray->point);

	float wp = dot(&ray_to_plane_delta, &plane_normal);
	float vp = dot(&ray_delta, &plane_normal);
	float k = wp / vp;

	if(k < 0 || k > 1) {
		return 0;
	}

	Vector3 pos = scalar_mul(&ray_delta, k);
	pos = add(&ray->point, &pos);

	Vector3 v0, v1, v2, v3;
	copy_vector(&v0, &rectangle->position);
	copy_vector(&v1, &rectangle->position);
	copy_vector(&v2, &rectangle->position);
	copy_vector(&v3, &rectangle->position);

	float width_by_2 = rectangle->width / 2;
	float height_by_2 = rectangle->height / 2;
	v0.x -= width_by_2; v0.y += height_by_2;
	v1.x += width_by_2; v1.y += height_by_2;
	v2.x -= width_by_2; v2.y -= height_by_2;
	v3.x += width_by_2; v3.y -= height_by_2;

	/* First diagonal test */
	Vector3 v_len = sub(&v2, &v0);
	Vector3 p_len = sub(&pos, &v0);
	float t = dot(&p_len, &v_len) / dot(&v_len, &v_len);
	if(t < 0 || t > 1) {
		return 0;
	}

	/* Second diagonal test */
	v_len = sub(&v3, &v2);
	p_len = sub(&pos, &v2);
	t = dot(&p_len, &v_len) / dot(&v_len, &v_len);
	if(t < 0 || t > 1) {
		return 0;
	}

	copy_vector(result, &pos);
	return 1;
}

void draw_rectangle(Rectangle *rectangle, const Matrix4* view, const Matrix4* projection) {
	glUseProgram(rectangle->program);
	make_identity(&rectangle->model);

	/* Translation & Scaling */
	translate_matrix(&rectangle->model, rectangle->position.x, rectangle->position.y, rectangle->position.z);
	scale(&rectangle->model, rectangle->scale.x, rectangle->scale.y, rectangle->scale.z);
	/* Translation & Scaling */

	/* Rotation */
	Vector3 tmp;
	init_vector(&tmp, rectangle->position.x, rectangle->position.y, rectangle->position.z);
	translate_matrix(&rectangle->model, 0, 0, 0);
	rotate(&rectangle->model, &rectangle->rotation_axes, rectangle->angle_in_degree);
	translate_matrix(&rectangle->model, tmp.x, tmp.y, tmp.z);
	set_matrix4(rectangle->program, "model", &rectangle->model);
	/* Rotation */

	glBindTexture(GL_TEXTURE_2D, rectangle->texture_id);
	glBindVertexArray(rectangle->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void delete_rectangle(Rectangle *rectangle) {
	glDeleteVertexArrays(1, &rectangle->vao);
	glDeleteBuffers(1, &rectangle->vbo);
}
