#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "shader.h"
#include "cube_uv.h"
#include "rectangle.h"
#include "grid.h"

unsigned int window_width = 400;
unsigned int window_height = 400;
Matrix4 view;
Vector3 position, up, front;
int firstMouse = 1;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX, lastY;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Provide arguments...\n");
		printf("1: windowed\\fullscreen\n");
		return 0;
	}

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
	// glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glfwSwapInterval(1);

	/* tmp */
	int shader = compile_shader("..\\shaders\\v_shader_with_tex.shader", "..\\shaders\\f_shader_with_tex.shader");	

	CuboidUV cuboid_uv_1, cuboid_uv_2;
	make_cuboid_uv(&cuboid_uv_1, shader, "..\\data\\test.png");
	translate_cuboid_uv(&cuboid_uv_1, 5, 1.1f, 0);
	make_cuboid_uv(&cuboid_uv_2, shader, "..\\data\\test.png");
	translate_cuboid_uv(&cuboid_uv_2, -2, 1.1f, 0);

	Rectangle rect_1;
	make_rectangle(&rect_1, shader, "..\\data\\gray.png");
	scale_rectangle(&rect_1, 20, 20, 1);
	rotate_rectangle(&rect_1, 1, 0, 0, 90);

	Grid grid;
	make_grid(&grid, 10, 10, 2, 2);
	/* tmp */

	Matrix4 projection;
	make_identity(&view);
	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 100.0f);

	init_vector(&front, -0.49f, -0.56f, -0.67f);
	init_vector(&position, 36.77f, 42.26, 44.36f);
	init_vector(&up, 0, 1, 0);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		float before = clock();

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(shader, "view", &view);
		set_matrix4(shader, "projection", &projection);

		draw_rectangle(&rect_1, &view, &projection);
		draw_cuboid_uv(&cuboid_uv_1, &view, &projection);
		draw_cuboid_uv(&cuboid_uv_2, &view, &projection);
		draw_grid(&grid, &view, &projection);

		glfwSwapBuffers(window);

		float delta = clock() - before;
		if(delta > 0) {
			float fps = CLOCKS_PER_SEC / delta;
			// printf("fps: %0.2f\n", fps);
		}
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("\nEnded.\n");
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		print_vector(&position);
		print_vector(&front);
	}
}

void processInput(GLFWwindow *window) {
	float camera_speed = 0.5f;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		// position.x += camera_speed * front.x;
		// position.y += camera_speed * front.y;
		// position.z += camera_speed * front.z;
		position.z -= camera_speed;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		// position.x -= camera_speed * front.x;
		// position.y -= camera_speed * front.y;
		// position.z -= camera_speed * front.z;
		position.z += camera_speed;
	}
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	float camera_speed = 2.0f;
	position.x += (camera_speed * front.x) * yoffset;
	position.y += (camera_speed * front.y) * yoffset;
	position.z += (camera_speed * front.z) * yoffset;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
