#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "shader.h"
#include "triangle.h"
#include "rect.h"
#include "cube.h"

unsigned int window_width = 400;
unsigned int window_height = 400;
Matrix4 view;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

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

	glfwMaximizeWindow(window);	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(1);

	/* tmp */
	int shader = compile_shader("v_shader.shader", "f_shader.shader");	
	// Triangle triangle;
	// make_triangle(&triangle, shader);

	// Cuboid cuboid;
	// make_cuboid(&cuboid, shader, "data\\square.png");

	Rect rect;
	make_rect(&rect, shader, "data\\test.png");
	/* tmp */

	Matrix4 projection;
	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 100.0f);
	make_identity(&view);
	translate_matrix(&view, 0, 0, -10.0f);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		float before = clock();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;

		Vector3 position, target, up;
		init_vector(&position, camX, 0, camZ);
		init_vector(&target, 0, 0, 0);
		init_vector(&up, 0, 1, 0);
		view = look_at(&position, &target, &up);

		set_matrix4(shader, "view", &view);
		set_matrix4(shader, "projection", &projection);

		// draw_triangle(&triangle, &view, &projection);
		// draw_cuboid(&cuboid, &view, &projection);
		draw_rect(&rect, &view, &projection);

		glfwSwapBuffers(window);

		float delta = clock() - before;
		if(delta > 0) {
			float fps = CLOCKS_PER_SEC / delta;
			// printf("fps: %0.2f\n", fps);
		}
		glfwPollEvents();
	}

	// delete_triangle(&triangle);
	// delete_cuboid(&cuboid);
	glfwDestroyWindow(window);
	glfwTerminate();

	printf("\nEnded.\n");
	return 0;
}

void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
