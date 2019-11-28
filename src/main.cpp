#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <easy/profiler.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "shader.h"
#include "cube_uv.h"
#include "rectangle.h"
#include "grid.h"
#include "line.h"
#include "cube.h"

unsigned int window_width = 400;
unsigned int window_height = 400;
Matrix4 view, projection;
Vector3 position, up, front;
int firstMouse = 1;
float yaw = -90.0f;
float pitch = -40.0f;
float lastX, lastY;
Grid grid;
Cuboid cuboid_1;
int shader1, shader2;
int show_cursor;

static Line lines[50];
static Rectangle rectangles[50];
static Cuboid points[50];
static int lines_count;
static int rectangles_count;
static int points_count;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void add_line(Vector3 start, Vector3 end, float r, float g, float b);
void add_rectangle(Vector3* position, Vector3* scale, Vector3* rotation_axes, float angle_in_degree, const char* image, int shader);
void add_point(float x, float y, float z, const char* image);
char* combine_string(const char* str_1, const char* str_2);

const char* assets_path = "..\\..\\data\\";
const char* shaders_path = "..\\..\\shaders\\";
const char* profs_path = "..\\..\\profs\\";

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Provide arguments...\n");
		printf("1: windowed\\fullscreen\n");
		return 0;
	}

	EASY_PROFILER_ENABLE;
	EASY_BLOCK("Initialising ...");

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 16); // @Note: What is the recommended value?

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *video_mode = glfwGetVideoMode(monitor);
	window_width = video_mode->width;
	window_height = video_mode->height;

	GLFWwindow *window = NULL;
	if(strcmp(argv[1], "windowed") == 0) {
		window = glfwCreateWindow(window_width, window_height, "Hello, World", NULL, NULL);
	}
	else if(strcmp(argv[1], "fullscreen") == 0) {
		window = glfwCreateWindow(window_width, window_height, "Hello, World", monitor, NULL);
	}
	else {
		printf("Unknown argument %s provided.\n", argv[1]);
	}

	lastX = window_width / 2.0f;
	lastY = window_height / 2.0f;

	glfwMaximizeWindow(window);	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glfwSwapInterval(1);

	/* tmp */
	shader1 = compile_shader(combine_string(shaders_path, "v_shader_with_tex.shader"), combine_string(shaders_path, "f_shader_with_tex.shader"));	
	shader2 = compile_shader(combine_string(shaders_path, "v_shader.shader"), combine_string(shaders_path, "f_shader.shader"));

	make_cuboid(&cuboid_1, shader1, combine_string(assets_path, "rectangle_red.png"));
	translate_cuboid(&cuboid_1, -5, 5, 5);

	Rectangle rect_1;
	make_rectangle(&rect_1, shader1, combine_string(assets_path, "gray.png"));
	scale_rectangle(&rect_1, 20, 20, 1);
	rotate_rectangle(&rect_1, 1, 0, 0, 90);

	make_grid(&grid, 20, 20, 2, 2);
	translate_grid(&grid, 0, 0, 0);
	rotate_grid(&grid, 1, 0, 0, 90);
	/* tmp */

	/* init view & projection */
	make_identity(&view);
	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
	init_vector(&front, -0.49f, -0.56f, -0.67f);
	init_vector(&position, 36.77f, 42.26, 44.36f);
	init_vector(&up, 0, 1, 0);
	/* init view & projection */

	/* Init other variables */
	show_cursor = 1;
	lines_count = 0;
	rectangles_count = 0;
	points_count = 0;
	/* Init other variables */

	EASY_END_BLOCK;

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		// float before = clock();

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(shader1, "view", &view);
		set_matrix4(shader1, "projection", &projection);

		// draw_rectangle(&rect_1, &view, &projection);
		// draw_cuboid(&cuboid_1, &view, &projection);
		draw_grid(&grid, &view, &projection);

		// for(int i = 0; i < lines_count; ++i)
		// 	draw_line(&lines[i], &view, &projection);
		// for(int i = 0; i < rectangles_count; ++i)
		// 	draw_rectangle(&rectangles[i], &view, &projection);
		for(int i = 0; i < points_count; ++i)
			draw_cuboid(&points[i], &view, &projection);

		EASY_BLOCK("Swap Buffers");
		glfwSwapBuffers(window);
		EASY_END_BLOCK;

		// float delta = clock() - before;
		// if(delta > 0) {
		// 	float fps = CLOCKS_PER_SEC / delta;
		// 	// printf("fps: %0.2f\n", fps);
		// }
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	profiler::dumpBlocksToFile(combine_string(profs_path, "test.prof"));

	printf("\nlines_count: %d\n", lines_count);
	printf("rectangles_count: %d\n", rectangles_count);
	printf("Ended.\n");
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		show_cursor = !show_cursor;
		if(show_cursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(window, NULL);
			firstMouse = 1;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(window, mouse_callback);
		}
	}
	else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		translate_grid_by(&grid, 0, 1, 0);
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		translate_grid_by(&grid, 0, -1, 0);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Vector3 norm = normalize_to(xpos, ypos, window_width, window_height);
		Vector ray = compute_mouse_ray(norm.x, norm.y, &view, &projection);
		Vector3 from, to;
		init_vector(&from, ray.point.x, ray.point.y, ray.point.z);
		to = scalar_mul(&ray.direction, 200);
		to = add(&from, &to);
		add_line(from, to, 0, 1, 0);

		EASY_FUNCTION(profiler::colors::Blue);
		/* Testing cube_aabb */
		/* Testing cube_aabb */

		EASY_BLOCK("grid check");
		/* Testing grid*/
		Vector3 plane_point;
		int hit_plane = in_plane_point(&grid.box, &plane_point, &from, &to);
		add_point(plane_point.x, plane_point.y, plane_point.z, combine_string(assets_path, "rectangle_red.png"));
		if(hit_plane) {
			Vector3 grid_hit_point;
			int hit_grid = get_sub_grid_mid_point(&grid, &plane_point, &grid_hit_point);
			if(hit_grid) {
				Vector3 scale, point;
				init_vector(&scale, 1, 1, 1);
				init_vector(&point, grid_hit_point.x, grid_hit_point.z, grid_hit_point.y + 0.2f);
				add_rectangle(&point, &scale, &grid.rotation_axes, grid.angle_in_degree, combine_string(assets_path, "red.png"), shader1);

				translate_cuboid(&cuboid_1, grid_hit_point.x, cuboid_1.position.y, grid_hit_point.z);
			}
		}
		/* Testing grid*/
		EASY_END_BLOCK;
	}
}

void processInput(GLFWwindow *window) {
	float camera_speed = 0.5f;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	// if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
	// 	position.x += camera_speed * front.x;
	// 	position.y += camera_speed * front.y;
	// 	position.z += camera_speed * front.z;
	// }
	// if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
	// 	position.x -= camera_speed * front.x;
	// 	position.y -= camera_speed * front.y;
	// 	position.z -= camera_speed * front.z;
	// }
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Vector3 res = cross(&front, &up);
		normalize_vector(&res);

		position.x -= camera_speed * res.x;
		position.y -= camera_speed * res.y;
		position.z -= camera_speed * res.z;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Vector3 res = cross(&front, &up);
		normalize_vector(&res);

		position.x += camera_speed * res.x;
		position.y += camera_speed * res.y;
		position.z += camera_speed * res.z;
	}
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Vector3 _1 = cross(&front, &up);
		Vector3 res = cross(&_1, &up);
		position.x -= camera_speed * res.x;
		position.y -= camera_speed * res.y;
		position.z -= camera_speed * res.z;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Vector3 _1 = cross(&front, &up);
		Vector3 res = cross(&_1, &up);
		position.x += camera_speed * res.x;
		position.y += camera_speed * res.y;
		position.z += camera_speed * res.z;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = 0;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	front.x = cos(to_radians(yaw)) * cos(to_radians(pitch));
	front.y = sin(to_radians(pitch));
	front.z = sin(to_radians(yaw)) * cos(to_radians(pitch));
	normalize_vector(&front);
}

void add_line(Vector3 start, Vector3 end, float r, float g, float b) {
	lines_count += 1;
	if(lines_count > 50) {
		printf("No more lines to use!!!!!!!!!!\n");
		return;
	}
	int index = lines_count - 1;

	make_line(&lines[index], &start, &end, shader2);
	init_vector(&lines[index].color, r, g, b);
}

void add_rectangle(Vector3* position, Vector3* scale, Vector3* rotation_axes, float angle_in_degree, const char* image, int shader) {
	rectangles_count += 1;
	if(rectangles_count > 50) {
		printf("No more rectangles to use!!!!!!!!!!\n");
		return;
	}

	int index = rectangles_count - 1;

	make_rectangle(&rectangles[index], shader, image);
	translate_rectangle(&rectangles[index], position->x, position->y, position->z);
	rotate_rectangle(&rectangles[index], rotation_axes->x, rotation_axes->y, rotation_axes->z, angle_in_degree);
	scale_rectangle(&rectangles[index], scale->x, scale->y, scale->z);
}

void add_point(float x, float y, float z, const char* image) {
	points_count += 1;
	if(points_count > 50) {
		printf("No more points to use!!!!!!!!!!\n");
		return;
	}

	int index = points_count - 1;
	make_cuboid(&points[index], shader1, image);
	scale_cuboid(&points[index], 0.1f, 0.1f, 0.1f);
	translate_cuboid(&points[index], x, y, z);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	float camera_speed = 2.0f;
	position.x += (camera_speed * front.x) * yoffset;
	position.y += (camera_speed * front.y) * yoffset;
	position.z += (camera_speed * front.z) * yoffset;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

char* combine_string(const char* str_1, const char* str_2) {
	unsigned int len = 100;
	char* str = (char*)malloc(sizeof(char) * len);

	strcpy(str, str_1);
	strcat(str, str_2);

	return str;
}
