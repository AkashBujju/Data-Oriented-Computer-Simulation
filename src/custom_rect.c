#include <glad/glad.h>
#include <stdlib.h>
#include <math.h>
#include "custom_rect.h"
#include "line.h"
#include "cube.h"
#include "shader.h"

extern const char* assets_path;
extern char* combine_string(const char*, const char*);
extern Line lines[50];
extern int lines_count;
// extern Cuboid points[50];
// extern int points_count;
// extern void add_point(float x, float y, float z, float scale, Cuboid* points_arr, int *current_total);

static Vector3 get_centroid_of_triangle(Vector3 *p1, Vector3 *p2, Vector3 *p3);
static Vector3 get_centroid_of_quad(CustomRect *custom_rect);

/*
 * p1 -> lower_left
 * p2 -> upper_left
 * p3 -> upper_right
 * p4 -> lower_right
*/

void make_custom_rect(CustomRect *custom_rect, Vector3 *p1, Vector3 *p2, Vector3 *p3, Vector3 *p4, int program, int texture_id, Vector2 *t_p1, Vector2 *t_p2, Vector2 *t_p3, Vector2 *t_p4) {
	custom_rect->program = program;

	custom_rect->ll = *p1;
	custom_rect->ul = *p2;
	custom_rect->ur = *p3;
	custom_rect->lr = *p4;

	custom_rect->t_ll = *t_p1;
	custom_rect->t_ul = *t_p2;
	custom_rect->t_ur = *t_p3;
	custom_rect->t_lr = *t_p4;

	float *vertices = (float*)malloc(sizeof(float) * 30);
	vertices[0] = p1->x; vertices[1] = p1->y; vertices[2] = p1->z; vertices[3] = t_p1->x; vertices[4] = t_p1->y;
	vertices[5] = p2->x; vertices[6] = p2->y; vertices[7] = p2->z; vertices[8] = t_p2->x; vertices[9] = t_p2->y;
	vertices[10] = p3->x; vertices[11] = p3->y; vertices[12] = p3->z; vertices[13] = t_p3->x; vertices[14] = t_p3->y;
	vertices[15] = p1->x; vertices[16] = p1->y; vertices[17] = p1->z; vertices[18] = t_p1->x; vertices[19] = t_p1->y;
	vertices[20] = p4->x; vertices[21] = p4->y; vertices[22] = p4->z; vertices[23] = t_p4->x; vertices[24] = t_p4->y;
	vertices[25] = p3->x; vertices[26] = p3->y; vertices[27] = p3->z; vertices[28] = t_p3->x; vertices[29] = t_p3->y;

	glGenVertexArrays(1, &custom_rect->vao);
	glGenBuffers(1, &custom_rect->vbo);
	glBindVertexArray(custom_rect->vao);
	glBindBuffer(GL_ARRAY_BUFFER, custom_rect->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 30, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	custom_rect->texture_id = texture_id;
	init_matrix(&custom_rect->model);
	custom_rect->position = get_centroid_of_quad(custom_rect);
	// add_point(custom_rect->position.x, custom_rect->position.y, custom_rect->position.z, 0.03f, points, &points_count);

	free(vertices);
}

void translate_custom_rect(CustomRect *custom_rect, float x, float y, float z) {
	custom_rect->position.x = x;
	custom_rect->position.y = y;
	custom_rect->position.z = z;
}

int onSegment(Vector3 p, Vector3 q, Vector3 r) {
    if (q.x <= f_max(p.x, r.x) && q.x >= f_min(p.x, r.x) && 
        q.y <= f_max(p.y, r.y) && q.y >= f_min(p.y, r.y) &&
		  q.z <= f_max(p.z, r.z) && q.z >= f_min(p.z, r.z))
        return 1;
    return 0;
} 

int orientation(Vector3 p, Vector3 q, Vector3 r, Vector3 normal) {
	Vector3 sub_1 = sub(&q, &p);
	Vector3 sub_2 = sub(&r, &p);
	Vector3 cross_res = cross(&sub_1, &sub_2);
	float res = dot(&normal, &cross_res);

	if(res >= -0.0001f && res <= 0.0001f)
		return 0;
	else if(res > 0)
		return 2;

	return 1;
}

int doIntersect(Vector3 p1, Vector3 q1, Vector3 p2, Vector3 q2, Vector3 plane_normal) {
    int o1 = orientation(p1, q1, p2, plane_normal); 
    int o2 = orientation(p1, q1, q2, plane_normal); 
    int o3 = orientation(p2, q2, p1, plane_normal); 
    int o4 = orientation(p2, q2, q1, plane_normal); 

    if (o1 != o2 && o3 != o4) 
        return 1;
  
    if (o1 == 0 && onSegment(p1, p2, q1)) return 1;
    if (o2 == 0 && onSegment(p1, q2, q1)) return 1; 
    if (o3 == 0 && onSegment(p2, p1, q2)) return 1; 
    if (o4 == 0 && onSegment(p2, q1, q2)) return 1; 
  
    return 0;
} 

int isInside(Vector3 polygon[], int n, Vector3 p, Vector3 plane_normal) {
    if (n < 3)  return 0;
	 Vector3 extreme = sub(&polygon[2], &polygon[1]);
	 extreme = scalar_mul(&extreme, 1000000);
	 extreme = add(&polygon[2], &extreme);
  
    int count = 0, i = 0; 
    do {
        int next = (i + 1) % n; 
  
        if (doIntersect(polygon[i], polygon[next], p, extreme, plane_normal)) {
            if (orientation(polygon[i], p, polygon[next], plane_normal) == 0) 
               return onSegment(polygon[i], p, polygon[next]); 
  
            count++; 
        } 
        i = next; 
    } while (i != 0); 
  
    return count & 1;
} 

int i_custom_rects(CustomRect *custom_rects, int count, Vector *ray, Vector3* result, Vector3* filter_axes) {
	Vector3 ray_end;
	ray_end = scalar_mul(&ray->direction, 200);
	ray_end = add(&ray->point, &ray_end);

	unsigned int hit_indices[10];
	unsigned int hit_count = 0;
	for(unsigned int i = 0; i < count; ++i) {
		Vector3 vec1 = sub(&custom_rects[i].ur, &custom_rects[i].ul);
		Vector3 vec2 = sub(&custom_rects[i].ul, &custom_rects[i].ll);
		Vector3 plane_normal = cross(&vec1, &vec2);
		normalize_vector(&plane_normal);

		Vector3 ray_delta = sub(&ray_end, &ray->point);
		Vector3 ray_to_plane_delta = sub(&custom_rects[i].position, &ray->point);

		float wp = dot(&ray_to_plane_delta, &plane_normal);
		float vp = dot(&ray_delta, &plane_normal);
		float k = wp / vp;

		if(k < 0 || k > 1)
			continue;

		Vector3 pos = scalar_mul(&ray_delta, k);
		pos = add(&ray->point, &pos);
		copy_vector(result, &pos);

		Vector3 points[4] = {custom_rects[i].ll, custom_rects[i].ul, custom_rects[i].ur, custom_rects[i].lr};
		int res = isInside(points, 4, pos, plane_normal);
		if(res == 1)
			hit_indices[hit_count++] = i;
	}

	if(hit_count == 0)
		return -1;
	else if(hit_count == 1)
		return hit_indices[0];

	/* Sorting */
	float min_distance = 999999;
	unsigned min_distance_index = 0;
	Vector3 from_point;

	for(unsigned int i = 0; i < hit_count; ++i) {
		CustomRect *c = &custom_rects[hit_indices[i]];
		init_vector(&from_point, c->position.x, c->position.y, c->position.z);
		if(filter_axes->x == 1)
			from_point.x = ray->point.x;
		else if(filter_axes->y == 1)
			from_point.y = ray->point.y;
		else if(filter_axes->z == 1)
			from_point.z = ray->point.z;

		float distance = sqrt(
			((c->position.x - from_point.x) * (c->position.x - from_point.x)) +
			((c->position.y - from_point.y) * (c->position.y - from_point.y)) +
			((c->position.z - from_point.z) * (c->position.z - from_point.z))
		);

		if(distance < min_distance) {
			min_distance = distance;
			min_distance_index = hit_indices[i];
		}
	}
	/* Sorting */

	return min_distance_index;
}

int i_custom_rect(CustomRect *custom_rect, Vector *ray, Vector3* result) {
	Vector3 vec1 = sub(&custom_rect->ur, &custom_rect->ul);
	Vector3 vec2 = sub(&custom_rect->ul, &custom_rect->ll);
	Vector3 plane_normal = cross(&vec1, &vec2);
	normalize_vector(&plane_normal);

	Vector3 ray_end;
	ray_end = scalar_mul(&ray->direction, 200);
	ray_end = add(&ray->point, &ray_end);

	Vector3 ray_delta = sub(&ray_end, &ray->point);
	Vector3 ray_to_plane_delta = sub(&custom_rect->position, &ray->point);

	float wp = dot(&ray_to_plane_delta, &plane_normal);
	float vp = dot(&ray_delta, &plane_normal);
	float k = wp / vp;

	if(k < 0 || k > 1)
		return 0;

	Vector3 pos = scalar_mul(&ray_delta, k);
	pos = add(&ray->point, &pos);
	copy_vector(result, &pos);

	Vector3 points[4] = {custom_rect->ll, custom_rect->ul, custom_rect->ur, custom_rect->lr};
	int res = isInside(points, 4, pos, plane_normal);
	
	return res;
}

static Vector3 get_centroid_of_triangle(Vector3 *p1, Vector3 *p2, Vector3 *p3) {
	Vector3 result;
	result.x = (p1->x + p2->x + p3->x) / 3;
	result.y = (p1->y + p2->y + p3->y) / 3;
	result.z = (p1->z + p2->z + p3->z) / 3;

	return result;
}

static Vector3 get_centroid_of_quad(CustomRect *custom_rect) {
	Vector3 result;

	/* Computing centroid */
	Vector3 centroid_1 = get_centroid_of_triangle(&custom_rect->ul, &custom_rect->ll, &custom_rect->lr);
	Vector3 centroid_2 = get_centroid_of_triangle(&custom_rect->ul, &custom_rect->ur, &custom_rect->lr);
	Vector3 centroid_3 = get_centroid_of_triangle(&custom_rect->ll, &custom_rect->ul, &custom_rect->ur);
	Vector3 centroid_4 = get_centroid_of_triangle(&custom_rect->ll, &custom_rect->ur, &custom_rect->lr);

	/* Computing centroid */
	LineEq line_1 = form_line(&centroid_1, &centroid_2);
	LineEq line_2 = form_line(&centroid_3, &centroid_4);
	result = line_intersect(&line_1, &line_2);

	return result;
}

void draw_custom_rect(CustomRect *custom_rect, const Matrix4* view, const Matrix4* projection) {
	glUseProgram(custom_rect->program);
	make_identity(&custom_rect->model);
	set_matrix4(custom_rect->program, "model", &custom_rect->model);

	glBindTexture(GL_TEXTURE_2D, custom_rect->texture_id);
	glBindVertexArray(custom_rect->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void delete_custom_rect(CustomRect *custom_rect) {
	glDeleteVertexArrays(1, &custom_rect->vao);
	glDeleteBuffers(1, &custom_rect->vbo);
}
