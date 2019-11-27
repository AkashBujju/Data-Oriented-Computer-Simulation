#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "cube.h"
#include "shader.h"
#include "utils.h"

extern const char* assets_path;
extern char* combine_string(const char*, const char*);

void make_cuboid(Cuboid *cuboid, int program, const char* image) {
	cuboid->program = program;

	float vertices[180];
	read_floats_from_file(combine_string(assets_path, "cuboid_vertices.dat"), vertices);

	glGenVertexArrays(1, &cuboid->vao);
	glGenBuffers(1, &cuboid->vbo);
	glBindVertexArray(cuboid->vao);

	glBindBuffer(GL_ARRAY_BUFFER, cuboid->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	cuboid->texture_id = make_texture(image);
	init_matrix(&cuboid->model);
	init_vector(&cuboid->position, 0, 0, 0);
	init_vector(&cuboid->scale, 1, 1, 1);
	init_vector(&cuboid->rotation_axes, 0, 0, 0);
	cuboid->angle_in_degree = 0;
	cuboid->width = 2;
	cuboid->height = 2;
	cuboid->depth= 2;
}

void translate_cuboid(Cuboid *cuboid, float x, float y, float z) {
	cuboid->position.x = x;
	cuboid->position.y = y;
	cuboid->position.z = z;
}

void rotate_cuboid(Cuboid* cuboid, float x, float y, float z, float degree) {
	cuboid->angle_in_degree = degree;
	cuboid->rotation_axes.x = x;
	cuboid->rotation_axes.y = y;
	cuboid->rotation_axes.z = z;
}

void scale_cuboid(Cuboid* cuboid, float x, float y, float z) {
	cuboid->scale.x = x;
	cuboid->scale.y = y;
	cuboid->scale.z = z;
	cuboid->width = 2 * x;
	cuboid->height = 2 * y;
	cuboid->depth = 2 * z;
}

void test_aabb(Cuboid *cuboid, Vector *ray) {
	Vector3 from, to;
	init_vector(&from, ray->point.x, ray->point.y, ray->point.z);
	to = scalar_mul(&ray->direction, 200);
	to = add(&from, &to);

	Vector3 min, max;
	float width_by_2 = cuboid->width / 2;
	float height_by_2 = cuboid->height / 2;
	float depth_by_2 = cuboid->depth / 2;

	init_vector(&min, cuboid->position.x - width_by_2, cuboid->position.y - height_by_2, cuboid->position.z - depth_by_2);
	init_vector(&max, cuboid->position.x + width_by_2, cuboid->position.y + height_by_2, cuboid->position.z + depth_by_2);

	Vector3 ray_dir = sub(&to, &from);
	float min_x = (min.x - from.x) / ray_dir.x;
	float max_x = (max.x - from.x) / ray_dir.x;
	if(max_x < min_x) { float tmp = max_x; max_x = min_x; min_x = tmp; }

	float min_y = (min.y - from.y) / ray_dir.y;
	float max_y = (max.y - from.y) / ray_dir.y;
	if(max_y < min_y) { float tmp = max_y; max_y = min_y; min_y = tmp; }

	float min_z = (min.z - from.z) / ray_dir.z;
	float max_z = (max.z - from.z) / ray_dir.z;
	if(max_z < min_z) { float tmp = max_z; max_z = min_z; min_z = tmp; }

	float t_min = (min_x > min_y) ? min_x : min_y;
	float t_max = (max_x < max_y) ? max_x : max_y;

	if(min_x > max_y || min_y > max_x || t_min > max_z || t_min > t_max) {
		return;
	}

	if(min_z > t_min)
		t_min = min_z;
	if(max_z < t_max)
		t_max = max_z;
	
	Vector3 p1, p2;
	p1 = scalar_mul(&ray_dir, t_min);
	p1 = add(&from, &p1);
	p2 = scalar_mul(&ray_dir, t_max);
	p2 = add(&from, &p2);
}

void draw_cuboid(Cuboid *cuboid, const Matrix4* view, const Matrix4* projection) {
	glUseProgram(cuboid->program);

	make_identity(&cuboid->model);
	translate_matrix(&cuboid->model, cuboid->position.x, cuboid->position.y, cuboid->position.z);
	scale(&cuboid->model, cuboid->scale.x, cuboid->scale.y, cuboid->scale.z);
	rotate(&cuboid->model, &cuboid->rotation_axes, cuboid->angle_in_degree);
	set_matrix4(cuboid->program, "model", &cuboid->model);

	glBindTexture(GL_TEXTURE_2D, cuboid->texture_id);
	glBindVertexArray(cuboid->vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void delete_cuboid(Cuboid *cuboid) {
	glDeleteVertexArrays(1, &cuboid->vao);
	glDeleteBuffers(1, &cuboid->vbo);
}
