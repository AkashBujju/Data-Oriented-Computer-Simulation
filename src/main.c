#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "shader.h"
#include "model.h"
#include "text.h"
#include "cube.h"
#include "line.h"
#include "sim.h"
#include "hash_table.h"
#include "floyd_warshall.h"

static unsigned int window_width = 400;
static unsigned int window_height = 400;
static Matrix4 view, projection, text_projection;
static Vector3 position, up, front;
static int firstMouse = 1;
static float yaw = -90.0f;
static float pitch = -40.0f;
static float lastX, lastY;
static int show_cursor;
int shader1, shader2, text_shader;

static Font font;
static Cuboid origin;
static Cuboid* cuboid_paths;
static int cuboid_paths_len;
static Line axes[3];
// static Grid grid;

/* Textures */
int violet_texture;
int gray_texture;
int road_texture;
int junction_texture;
int signal_red_texture;
/* Textures */

/* Modes */
/* Modes */

/* Display Text */
static char input_text[100];
static int input_mode = 0;
static int show_origin = 1;
static float zoom_speed = 1.0f;
/* Display Text */

/* Paths */
Paths *paths = NULL;
/* Paths */

/* Models */
#define TOTAL_ROADS 150 + 9 * 3 * 4
#define ROADS_LIMIT 512
Model *roads[TOTAL_ROADS];
Road _roads[ROADS_LIMIT];
unsigned int road_keys[ROADS_LIMIT];
unsigned int road_id_start, road_id_end;

#define TOTAL_JUNCTIONS 40 * 5
#define TOTAL_WORKING_JUNCTIONS 40
#define JUNCTIONS_LIMIT 256
Model *junctions[TOTAL_JUNCTIONS];
Junction _junctions[JUNCTIONS_LIMIT];
unsigned int junction_keys[JUNCTIONS_LIMIT];
unsigned int junction_id_start, junction_id_end;

#define TOTAL_SIGNALS 40 * 4
#define SIGNALS_LIMIT 256
Model *signals[TOTAL_SIGNALS];
Signal _signals[SIGNALS_LIMIT];
unsigned int signal_keys[SIGNALS_LIMIT];
unsigned int signal_id_start, signal_id_end;
/* Models */

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
char* combine_string(const char* str_1, const char* str_2);
void character_callback(GLFWwindow* window, unsigned int codepoint);
Model* load_model(const char* filename, int texture_id);
void remove_last_char(char* text);
void process_command(char* command);
void append_char(char* str, char c);
float lerp(float val_1, float val_2, float t);
float distance(Vector3 *p1, Vector3 *p2);
int n_lerp(Vector3 *start, Vector3 *current, Vector3 *destination, float *vel, float acc, float dcc, float top_vel);
void init_sim();
void init_paths();
Vector3 get_position_of_id(int id);
int is_road(int id);
int is_junction(int id);
int is_signal(int id);

const char* assets_path = "../../data/";
const char* shaders_path = "../../shaders/";

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Provide arguments...\n");
		printf("1: windowed\\fullscreen\n");
		return 0;
	}

	printf("Main called\n");

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, 1); // @Note: On intel machines antialiasing has to be turned on.

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
	glfwSetCharCallback(window, character_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1);

	/* Load textures */
	{
		violet_texture = make_texture(combine_string(assets_path, "png/safety_blue.png"));
		gray_texture = make_texture(combine_string(assets_path, "png/gray.png"));
		signal_red_texture = make_texture(combine_string(assets_path, "png/red_signal.png"));
		road_texture = make_texture(combine_string(assets_path, "png/road.png"));
		junction_texture = make_texture(combine_string(assets_path, "png/junction_gimp.png"));
	}

	/* Init shaders */
	{
		shader1 = compile_shader(combine_string(shaders_path, "v_shader_with_tex.shader"), combine_string(shaders_path, "f_shader_with_tex.shader"));	
		shader2 = compile_shader(combine_string(shaders_path, "v_shader.shader"), combine_string(shaders_path, "f_shader.shader"));
		text_shader = compile_shader(combine_string(shaders_path, "v_text.shader"), combine_string(shaders_path, "f_text.shader"));
	}

	{
		make_cuboid(&origin, shader1, violet_texture);
		scale_cuboid(&origin, 0.1f, 0.1f, 0.1f);

		Vector3 x_end, y_end, z_end;
		init_vector(&x_end, 100, 0, 0);
		init_vector(&y_end, 0, 100, 0);
		init_vector(&z_end, 0, 0, 100);
		make_line(&axes[0], &origin.position, &x_end, shader2);
		make_line(&axes[1], &origin.position, &y_end, shader2);
		make_line(&axes[2], &origin.position, &z_end, shader2);
		init_vector(&axes[0].color, 1, 0, 0);
		init_vector(&axes[1].color, 0, 1, 0);
		init_vector(&axes[2].color, 0, 0, 1);
	}

	/* Init grid */
	// {
	// 	make_grid(&grid, 100, 100, 0.2f, 0.2f);
	// 	translate_grid(&grid, 0, 0, 0);
	// 	rotate_grid(&grid, 1, 0, 0, 90);
	// 	rotate_rectangle(&grid.background, 1, 0, 0, 90);
	// 	scale_rectangle(&grid.background, 10, 10, 10);
	// }
	/* Init grid */

	/* init view & projection */
	{
		make_identity(&view);
		projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
		text_projection = ortho(0, 1440.0f, 0, 900);
		init_vector(&front, -0.49f, -0.56f, -0.67f);
		init_vector(&position, 11.0f, 13.0, 15.0f);
		init_vector(&up, 0, 1, 0);
	}

	/* Init other variables */
	{
		show_cursor = 1;
	}
	/* Init other variables */

	/* Init font */
	FT_Library ft;
	init_freetype(&ft);
	init_font(&font, combine_string(assets_path, "fonts/consolas.ttf"), &ft);

	/* Init font */

	/* Loading models */
	// Model *car_1 = load_model("car_1.model");
	// translate_model(car_1, 0.25f, 0.4f, 0);
	// scale_model(car_1, 0.4f, 0.4f, 0.4f);

	// Model *car_2 = load_model("car_2.model");
	// translate_model(car_2, 0.25f, 0.4f, 10);
	// scale_model(car_2, 0.4f, 0.4f, 0.4f);
	
	init_sim();
	/* Loading models */

	float fps = 0;
	// float start = clock();

	// Car c1, c2;
	// {
	// 	c1.id = 1;
	// 	c1.should_stop = 0;
	// 	c1.current_relax_secs = 0;
	// 	c1.vel = 0.01;
	// 	copy_vector(&c1.start_position, &car_1->position);
	// 	copy_vector(&c1.position, &car_1->position);
	// 	copy_vector(&c1.to_follow_position, &car_2->position);

	// 	c2.id = 2;
	// 	c2.should_stop = 0;
	// 	c2.current_relax_secs = 0;
	// 	c2.vel = 0.01;
	// 	Vector3 to_follow;
	// 	copy_vector(&to_follow, &car_2->position);
	// 	to_follow.z += 50;
	// 	copy_vector(&c2.start_position, &car_2->position);
	// 	copy_vector(&c2.position, &car_2->position);
	// 	copy_vector(&c2.to_follow_position, &to_follow);
	// }

	// Vector3 start_pos;
	// copy_vector(&start_pos, &origin.position);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		float before = clock();
		// float now = (clock() - start) / CLOCKS_PER_SEC;

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// {
		// 	if((now - c1.current_relax_secs) > 1) {
		// 		c1.should_stop = 0;
		// 	}

		// 	if(!c1.should_stop) {
		// 		int res = n_lerp(&c1.start_position, &c1.position, &c1.to_follow_position, &c1.vel, 0.001, 0.001, 3);
		// 		if(res) {
		// 			c1.current_relax_secs = now;
		// 			c1.should_stop = 1;
		// 		}
		// 		else {
		// 			c1.position.z += c1.vel;
		// 		}
		// 	}

		// 	copy_vector(&c1.to_follow_position, &car_2->position);
		// 	c1.to_follow_position.z -= 0.5f;
		// 	copy_vector(&car_1->position, &c1.position);
		// }

		// {
		// 	if((now - c2.current_relax_secs) > 1) {
		// 		c2.should_stop = 0;
		// 	}

		// 	if(!c2.should_stop) {
		// 		int res = n_lerp(&c2.start_position, &c2.position, &c2.to_follow_position, &c2.vel, 0.001, 0.001, 3);
		// 		if(res) {
		// 			c2.current_relax_secs = now;
		// 			c2.should_stop = 1;
		// 		}
		// 		else {
		// 			c2.position.z += c2.vel;
		// 		}
		// 	}

		// 	// copy_vector(&c2.to_follow_position, &car_2->position);
		// 	copy_vector(&car_2->position, &c2.position);
		// }

		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(shader1, "view", &view);
		set_matrix4(shader1, "projection", &projection);
		set_matrix4(text_shader, "projection", &text_projection);

		if(input_mode) {
			render_text(&font, text_shader, "input_mode: on", 0.02f * window_width, 0.1f * window_height, 1, 0, 1, 0);
			render_text(&font, text_shader, "-> ", 0.02f * window_width, 0.05f * window_height, 1, 0, 1, 0);
			render_text(&font, text_shader, input_text, 0.04f * window_width, 0.05f * window_height, 1, 0.8f, 0, 0);
		}
		else {
			render_text(&font, text_shader, "input_mode: off", 0.02f * window_width, 0.1f * window_height, 1, 0, 1, 0);
		}

		/* FPS */
		{
			char fps_str[20];
			char tmp[10];
			gcvt(fps, 5, tmp);

			strcpy(fps_str, "fps: ");
			strcat(fps_str, tmp);
			render_text(&font, text_shader, fps_str, 0.02f * window_width, 0.9f * window_height, 1, 0, 1, 0);
		}

		/* Camera Position */
		{
			char camera_position_str[50];
			char tmp[10];

			gcvt(position.x, 3, tmp);
			strcpy(camera_position_str, "camera: ");
			strcat(camera_position_str, tmp);

			gcvt(position.y, 3, tmp);
			strcat(camera_position_str, ", ");
			strcat(camera_position_str, tmp);

			gcvt(position.z, 3, tmp);
			strcat(camera_position_str, ", ");
			strcat(camera_position_str, tmp);
			render_text(&font, text_shader, camera_position_str, 0.02f * window_width, 0.85f * window_height, 1, 0, 1, 0);
		}

		/* Zoom Speed */
		{
			char zoom_speed_str[20];
			char tmp[10];
			gcvt(zoom_speed, 4, tmp);

			strcpy(zoom_speed_str, "zoom_speed: ");
			strcat(zoom_speed_str, tmp);
			render_text(&font, text_shader, zoom_speed_str, 0.02f * window_width, 0.80f * window_height, 1, 0, 1, 0);
		}

		if(show_origin) {
			draw_cuboid(&origin, &view, &projection);
			draw_line(&axes[0], &view, &projection);
			draw_line(&axes[1], &view, &projection);
			draw_line(&axes[2], &view, &projection);
		}

		for(int i = 0; i < cuboid_paths_len; ++i) {
			draw_cuboid(&cuboid_paths[i], &view, &projection);
		}

		// if(toggle_grid) {
		// 	draw_grid(&grid, &view, &projection);
		// }

		{
			// draw_model(car_1, &view, &projection);
			// draw_model(car_2, &view, &projection);

			for(int i = 0; i < TOTAL_ROADS; ++i) {
				draw_model(roads[i], &view, &projection);
			}
			for(int i = 0; i < TOTAL_JUNCTIONS; ++i) {
				draw_model(junctions[i], &view, &projection);
			}
			for(int i = 0; i < TOTAL_SIGNALS; ++i) {
				draw_model(signals[i], &view, &projection);
			}
		}

		glfwSwapBuffers(window);

		float delta = clock() - before;
		if(delta > 0) {
			fps = CLOCKS_PER_SEC / delta;
		}
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("Ended.\n");
	return 0;
}

void init_sim() {
	/* CONNECT ID ROADS VERTICALLY */
	unsigned int current_id = 1;
	unsigned int current_index = 0;
	const unsigned int start_junction_id = 1 + (2 * (TOTAL_ROADS - 1)) + 2;
	road_id_start = current_id;
	for(int i = 1; i <= 10; ++i) {
		for(int j = 1; j <= 15; ++j) {
			Road road;
			road.left_lane_id = current_id;
			road.right_lane_id = current_id + 1;

			if(j == 1) {
				road.to_forward_id = current_id + 2;
				road.to_backward_id = -1;
			}
			else if(j == 15) {
				road.to_forward_id = -1;
				road.to_backward_id = current_id - 1;
			}
			else if(j % 3 == 0) {
				road.to_forward_id = start_junction_id + (j / 3) + ((i - 1) * 4) - 1; // @Note: '4' is the number of junction 'rows'.
				road.to_backward_id = current_id - 1;
			}
			else if(j != 1 && (j - 1) % 3 == 0) {
				road.to_backward_id = start_junction_id + floor(j / 3) + ((i - 1) * 4) - 1; // @Note: '4' is the number of junction 'rows'.
				road.to_forward_id = current_id + 2;
			}
			else {
				road.to_forward_id = current_id + 2;
				road.to_backward_id = current_id - 1;
			}

			put_road(road_keys, _roads, current_id, &road, ROADS_LIMIT);
			current_id += 2;
			current_index += 1;
		}
	}
	/* CONNECT ID ROADS VERTICALLY */

	/* CONNECT ID ROADS HORIZONTALLY */
	unsigned int junc_id_1 = start_junction_id;
	for(int i = 1; i <= 4; ++i) {
		for(int j = 1; j <= 27; ++j) {
			Road road;
			road.left_lane_id = current_id;
			road.right_lane_id = current_id + 1;

			if(j == 1 || (j - 1) % 3 == 0) {
				road.to_forward_id = current_id + 2;
				road.to_backward_id = junc_id_1;
				junc_id_1 += 4;
			}
			else if(j % 3 == 0) {
				road.to_forward_id = junc_id_1;
				road.to_backward_id = current_id - 1;
			}
			else {
				road.to_forward_id = current_id + 2;
				road.to_backward_id = current_id - 1;
			}

			put_road(road_keys, _roads, current_id, &road, ROADS_LIMIT);
			current_id += 2;
			current_index += 1;
		}
		junc_id_1 = start_junction_id + i;
	}
	road_id_end = current_id - 1;
	/* CONNECT ID ROADS HORIZONTALLY */

	/* CONNECT ID JUNCTIONS TO ROADS */
	current_index = 0;
	current_id = start_junction_id;
	junction_id_start = current_id;
	unsigned int vertical_road_start_id = 5; // @Hardcoded
	unsigned int horizontal_road_start_id = 150 * 2 + 1; // @Hardcoded
	const unsigned int start_signal_id = start_junction_id + TOTAL_WORKING_JUNCTIONS;
	unsigned int current_signal_id = start_signal_id;
	for(int i = 1; i <= 10; ++i) {
		for(int j = 1; j <= 4; ++j) {
			Junction junction;
			junction.id = current_id;
			junction.to_up_id = vertical_road_start_id + 2;
			junction.to_down_id = vertical_road_start_id + 1;
			junction.to_right_id = horizontal_road_start_id;
			junction.to_left_id = horizontal_road_start_id - 1;
			
			// @Note: The signals are placed TL-TR-BL-BR from the starting from the top-left
			junction.to_top_left_signal_id = current_signal_id;
			junction.to_top_right_signal_id = current_signal_id + 1;
			junction.to_down_left_signal_id = current_signal_id + 2;
			junction.to_down_right_signal_id = current_signal_id + 3;
			
			if(i == 1) {
				junction.to_left_id = -1;
			}
			else if(i == 10) {
				junction.to_right_id = -1;
			}

			put_junction(junction_keys, _junctions, current_id, &junction, JUNCTIONS_LIMIT);
			current_id += 1;
			current_index += 1;
			current_signal_id += 4;
			vertical_road_start_id += 6; // @Hardcoded
			if(j == 4) {
				vertical_road_start_id += 6; // @Hardcoded
			}

			horizontal_road_start_id += 27 * 2;
		}
		horizontal_road_start_id = (150 * 2 + 1) + (i * 6); // @Hardcoded
	}
	junction_id_end = current_id - 1;
	/* CONNECT ID JUNCTIONS TO ROADS */

	/* CONNECT SIGNAL TO JUNCTIONS */
	current_index = 0;
	current_signal_id = start_signal_id;
	signal_id_start = current_signal_id;
	unsigned int current_junction_id = start_junction_id;
	for(int i = 1; i <= 10; ++i) {
		for(int j = 1; j <= 4; ++j) {
			Signal signal_1, signal_2, signal_3, signal_4;
			signal_1.id = current_signal_id;
			signal_2.id = current_signal_id + 1;
			signal_3.id = current_signal_id + 2;
			signal_4.id = current_signal_id + 3;

			signal_1.main_junction_id = current_junction_id;
			signal_2.main_junction_id = current_junction_id;
			signal_3.main_junction_id = current_junction_id;
			signal_4.main_junction_id = current_junction_id;

			put_signal(signal_keys, _signals, current_signal_id, &signal_1, SIGNALS_LIMIT);
			put_signal(signal_keys, _signals, current_signal_id + 1, &signal_2, SIGNALS_LIMIT);
			put_signal(signal_keys, _signals, current_signal_id + 2, &signal_3, SIGNALS_LIMIT);
			put_signal(signal_keys, _signals, current_signal_id + 3, &signal_4, SIGNALS_LIMIT);

			current_index += 4;
			current_signal_id += 4;
			current_junction_id += 1;
		}
	}
	signal_id_end = current_signal_id - 1;
	/* CONNECT SIGNAL TO JUNCTIONS */

	float start_x = -21.6;
	float start_z = 10;
	float start_y = 0.15f;
	float x_by = 1.2f * 4;

	/* Laying vertical roads */
	unsigned int index = 0;
	for(int i = 1; i <= 10; ++i) {
		for(int j = 1; j <= 15; ++j) {
			roads[index] = load_model("road.model", road_texture);
			translate_model(roads[index], start_x, start_y, start_z);
			rotate_model(roads[index], 1, 0, 0, 90);
			scale_model(roads[index], 2, 2, 1);

			if(j % 3 == 0)
				start_z -= 2.4f;
			else
				start_z -= 1.2f;

			index += 1;
		}

		start_x += x_by;
		start_z = 10;
	}
	/* Laying vertical roads */

	/* Laying horizontal roads */
	start_x = -21.6 + 1.2f;
	start_z = 10 - 1.2f * 3;
	start_y = 0.15f;
	for(int i = 1; i <= 4; ++i) {
		for(int j = 1; j <= 27; ++j) {
			roads[index] = load_model("road.model", road_texture);
			translate_model(roads[index], start_x, start_y, start_z);
			rotate_model(roads[index], 1, 1, 1, 90);
			scale_model(roads[index], 2, 2, 1);
			
			start_x += 1.2f;
			if(j % 3 == 0) {
				start_x += 1.2f;
			}
			index += 1;
		}
		start_x = -21.6 + 1.2f;
		start_z -= 1.2f * 4;
	}
	/* Laying horizontal roads */

	/* Laying junctions and signals */
	start_x = -21.6;
	start_z = 10 - 1.2f * 3;
	start_y = 0.15f;
	index = 0;
	unsigned int signal_index = 0;
	for(int i = 0; i < 10; ++i) {
		for(int j = 0; j < 4; ++j) {
			junctions[index] = load_model("junction.model", junction_texture);
			translate_model(junctions[index], start_x, start_y, start_z);
			rotate_model(junctions[index], 1, 0, 0, 90);
			scale_model(junctions[index], 2, 2, 1);
			index += 1;

			junctions[index] = load_model("junction.model", junction_texture);
			translate_model(junctions[index], start_x - 0.75f, start_y, start_z - 0.75f);
			rotate_model(junctions[index], 1, 0, 0, 90);
			scale_model(junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			junctions[index] = load_model("junction.model", junction_texture);
			translate_model(junctions[index], start_x + 0.75f, start_y, start_z - 0.75f);
			rotate_model(junctions[index], 1, 0, 0, 90);
			scale_model(junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			junctions[index] = load_model("junction.model", junction_texture);
			translate_model(junctions[index], start_x - 0.75f, start_y, start_z + 0.75f);
			rotate_model(junctions[index], 1, 0, 0, 90);
			scale_model(junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			junctions[index] = load_model("junction.model", junction_texture);
			translate_model(junctions[index], start_x + 0.75f, start_y, start_z + 0.75f);
			rotate_model(junctions[index], 1, 0, 0, 90);
			scale_model(junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			signals[signal_index] = load_model("signal_red.model", signal_red_texture);
			translate_model(signals[signal_index], start_x - 0.75f, 0.5f, start_z - 0.75f);
			scale_model(signals[signal_index], 0.25f, 0.25f, 0.25f);
			signal_index += 1;

			signals[signal_index] = load_model("signal_red.model", signal_red_texture);
			translate_model(signals[signal_index], start_x + 0.75f, 0.5f, start_z - 0.75f);
			scale_model(signals[signal_index], 0.25f, 0.25f, 0.25f);
			rotate_model(signals[signal_index], 0, 1, 0, 90);
			signal_index += 1;

			signals[signal_index] = load_model("signal_red.model", signal_red_texture);
			translate_model(signals[signal_index], start_x - 0.75f, 0.5f, start_z + 0.75f);
			scale_model(signals[signal_index], 0.25f, 0.25f, 0.25f);
			rotate_model(signals[signal_index], 0, 1, 0, -90);
			signal_index += 1;

			signals[signal_index] = load_model("signal_red.model", signal_red_texture);
			translate_model(signals[signal_index], start_x + 0.75f, 0.5f, start_z + 0.75f);
			scale_model(signals[signal_index], 0.25f, 0.25f, 0.25f);
			rotate_model(signals[signal_index], 0, 1, 0, 180);
			signal_index += 1;

			start_z -= 1.2f * 4;
		}

		start_x += 1.2f * 4;
		start_z = 10 - 1.2f * 3;
	}
	/* Laying junctions and signals */

	/* Copying position from Model to _models */
	current_id = 1; // @Note: We need this to find the number of rows and columns of the matrix.
	for(int i = 0; i < TOTAL_ROADS; ++i) {
		Road *road = get_road(road_keys, _roads, current_id, ROADS_LIMIT);
		copy_vector(&road->position, &roads[i]->position);
		current_id += 2;
	}
	for(int i = 0; i < TOTAL_WORKING_JUNCTIONS; ++i) {
		// @Note: Here the junction are placed the order: center, top_left, top_right, bottom_left, bottom_right
		Junction *junction = get_junction(junction_keys, _junctions, current_id, JUNCTIONS_LIMIT);
		copy_vector(&junction->position, &junctions[i*5]->position); // @Note: i*5 because of the above note.
		current_id += 1;
	}
	for(int i = 0; i < TOTAL_SIGNALS; ++i) {
		Signal *signal= get_signal(signal_keys, _signals, current_id, SIGNALS_LIMIT);
		copy_vector(&signal->position, &signals[i]->position);
		current_id += 1;
	}
	/* Copying position from Model to _models */

	/* Filling the matrix */
	// @Note: We are not including the side junctions and signals in the matrix, as they are useless in pathfinding.
	const unsigned int num_rows = 1 + (TOTAL_ROADS) * 2 + TOTAL_WORKING_JUNCTIONS; // @Note: We ignore the first row and column of the matrix. Added '1'.
	int **matrix = (int**)malloc(sizeof(int*) * num_rows);
	current_id = 1;

	// Setting all indices to '0'
	for(unsigned int i = 0; i < num_rows; ++i) {
		matrix[i] = (int*)malloc(sizeof(int) * num_rows);
		for(unsigned int j = 0; j < num_rows; ++j) {
			matrix[i][j] = 0;
		}
	}

	for(int i = 0; i < TOTAL_ROADS; ++i) {
		Road *road = get_road(road_keys, _roads, current_id, ROADS_LIMIT);
		if(road->left_lane_id != -1) {
			matrix[road->left_lane_id][road->to_forward_id] = 1;
		}
		if(road->right_lane_id != -1) {
			matrix[road->right_lane_id][road->to_backward_id] = 1;
		}
		current_id += 2;
	}
	for(int i = 0; i < TOTAL_WORKING_JUNCTIONS; ++i) {
		Junction *junction = get_junction(junction_keys, _junctions, current_id, JUNCTIONS_LIMIT);
		if(junction->to_left_id != -1) {
			matrix[junction->id][junction->to_left_id] = 1;
		}
		if(junction->to_right_id != -1) {
			matrix[junction->id][junction->to_right_id] = 1;
		}
		matrix[junction->id][junction->to_up_id] = 1;
		matrix[junction->id][junction->to_down_id] = 1;
		current_id += 1;
	}
	/* Filling the matrix */

	/* Writing to file */
	FILE *file = fopen(combine_string(assets_path, "/matrix/graph.matrix"), "w");
	for(int i = 0; i < num_rows; ++i) {
		for(int j = 0; j < num_rows; ++j) {
			fprintf(file, "%d, ", matrix[i][j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);

	free(matrix); // @Note: We are not freeing the whole memory. For some reason free matrix[i] crashes!
	/* Filling the matrix */

	/* Init Paths */
	init_paths();
	/* Init Paths */
}

void init_paths() {
	char* p = combine_string(assets_path, "matrix/graph.matrix");
	Matrix *mat = load_matrix(p);
	convert_to_floyd_form(mat);
	paths = floyd_warshall(mat->mat, mat->len);

	Junction *junction = get_junction(junction_keys, _junctions, 521, JUNCTIONS_LIMIT);
	printf("junction_id: %d\n", junction->id);	
	printf("to_top_id: %d\n", junction->to_up_id);
	printf("to_down_id: %d\n", junction->to_down_id);
	printf("to_left_id: %d\n", junction->to_left_id);
	printf("to_right_id: %d\n", junction->to_right_id);
	
	int start_node = 301;
	int end_node = 2;
	int index = get(paths->keys, start_node, end_node, paths->limit);
	if(index != -1) {
		printf("index: %d\n", index);
		cuboid_paths_len = paths->len[index] - 2;
		cuboid_paths = (Cuboid*)malloc(sizeof(Cuboid) * cuboid_paths_len);
		for(int i = 2; i < paths->len[index]; ++i) {
			int node_id = paths->p[index][i];
			Vector3 pos = get_position_of_id(node_id);

			if(i == 2 || i == paths->len[index] - 1) {
				make_cuboid(&cuboid_paths[i - 2], shader1, gray_texture);
			}
			else {
				make_cuboid(&cuboid_paths[i - 2], shader1, violet_texture);
			}
			scale_cuboid(&cuboid_paths[i - 2], 0.1f, 0.1f, 0.1f);
			translate_cuboid(&cuboid_paths[i - 2], pos.x, pos.y + 0.5f, pos.z);

			printf("node_id: %d, position: %.2f %.2f %.2f\n", node_id, pos.x, pos.y, pos.z);
		}
		printf("\n");
	}

	// printf("road_id_start: %d, road_id_end: %d\n", road_id_start, road_id_end);
	// printf("junction_id_start: %d, junction_id_end: %d\n", junction_id_start, junction_id_end);
	// printf("signal_id_start: %d, signal_id_end: %d\n", signal_id_start, signal_id_end);
}

int is_road(int id) {
	if(id >= road_id_start && id <= road_id_end)
		return 1;
	return 0;
}

int is_junction(int id) {
	if(id >= junction_id_start && id <= junction_id_end)
		return 1;
	return 0;
}

int is_signal(int id) {
	if(id >= signal_id_start && id <= signal_id_end)
		return 1;
	return 0;
}

Vector3 get_position_of_id(int id) {
	if(id >= road_id_start && id <= road_id_end) {
		Road *road = NULL;
		if(id % 2 == 0)
			road = get_road(road_keys, _roads, id - 1, ROADS_LIMIT);
		else
			road = get_road(road_keys, _roads, id, ROADS_LIMIT);
		
		Vector3 pos = road->position;
		if(id >= 1 && id <= 300) {
			if(id % 2 == 0) {
				pos.x += 0.3f;
			}
			else {
				pos.x -= 0.3f;
			}
		}
		else {
			if(id % 2 == 0) {
				pos.z += 0.3f;
			}
			else {
				pos.z -= 0.3f;
			}
		}
		return pos;
	}
	else if(id >= junction_id_start && id <= junction_id_end) {
		Junction *junction = get_junction(junction_keys, _junctions, id, JUNCTIONS_LIMIT);
		return junction->position;
	}
	else {
		Signal *signal = get_signal(signal_keys, _signals, id, SIGNALS_LIMIT);
		return signal->position;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		if(input_mode)
			remove_last_char(input_text);
	}
	else if(key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		input_mode = !input_mode;	
	}
	else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		process_command(input_text);
		strcpy(input_text, "");
		input_mode = 0;
	}

	if(input_mode)
		return;

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
	// else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
	// 	toggle_grid = !toggle_grid;
	// }
}

void process_command(char* command) {
	char *split_str[2];
	char delim[2] = " ";
	char *ptr = strtok(command, delim);
	size_t len = strlen(ptr);
	split_str[0] = (char*)malloc(sizeof(char) * len);
	strcpy(split_str[0], ptr);

	ptr = strtok(NULL, delim);
	len = strlen(ptr);
	split_str[1] = (char*)malloc(sizeof(char) * len);
	strcpy(split_str[1], ptr);

	if(strcmp(split_str[0], "toggle") == 0) {
		if(strcmp(split_str[1], "origin") == 0) {
			show_origin = !show_origin;
		}
	}
	else if(strcmp(split_str[0], "zoom_speed") == 0) {
		zoom_speed = strtof(split_str[1], NULL);
	}
}

Model* load_model(const char* filename, int texture_id) {
	FILE *file = NULL;
	const char* path = "../../data/models/";
	char final_path[50];
	strcpy(final_path, path);
	strcat(final_path, filename);
	file = fopen(final_path, "r");

	if(file == NULL) {
		printf("Could not open file %s\n", final_path);
		return NULL;
	}
	else {
		char tmp[50];

		int total_floats;
		fscanf(file, "%s", tmp);
		fscanf(file, "%d", &total_floats);

		int total_vertices; // OpenGL vertices
		fscanf(file, "%s", tmp);
		fscanf(file, "%d", &total_vertices);

		float *vertices = (float*)malloc(sizeof(float) * total_floats);
		char c;
		for(int i = 0; i < total_floats; ++i) {
			fscanf(file, "%f", &vertices[i]);
			fscanf(file, "%c", &c);
		}

		Vector3 local_origin;
		fscanf(file, "%s", tmp); // bounding_box
		fscanf(file, "%s", tmp); // local_origin
		fscanf(file, "%f", &local_origin.x);
		fscanf(file, "%c", &c); // ,
		fscanf(file, "%f", &local_origin.y);
		fscanf(file, "%c", &c); // ,
		fscanf(file, "%f", &local_origin.z);
		fscanf(file, "%c", &c);

		float width, height, depth;
		fscanf(file, "%s", tmp); // width
		fscanf(file, "%f", &width);
		fscanf(file, "%s", tmp); // height
		fscanf(file, "%f", &height);
		fscanf(file, "%s", tmp); // depth
		fscanf(file, "%f", &depth);

		Model *model = (Model*)malloc(sizeof(Model));
		make_model(model, shader1, texture_id, vertices, total_floats, total_vertices, &local_origin, width, height, depth);

		// printf("\nmodel_loaded %s.\n", filename);
		free(vertices);
		fclose(file);

		return model;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Vector3 norm = normalize_to(xpos, ypos, window_width, window_height);
		Vector ray = compute_mouse_ray_2(norm.x, norm.y, &view, &projection);
		Vector3 from, to;
		init_vector(&from, ray.point.x, ray.point.y, ray.point.z);
		to = scalar_mul(&ray.direction, 200);
		to = add(&from, &to);
	}
}

void processInput(GLFWwindow *window) {
	float camera_speed = 0.5f;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
	if(input_mode)
		return;

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

void append_char(char* str, char c) {
	unsigned int index = 0;
	while(1) {
		if(str[index] == '\0') {
			str[index] = c;
			str[index + 1] = '\0';
			break;
		}

		index += 1;
	}
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	if(input_mode) {
		char c = (char)codepoint;
		append_char(input_text, c);
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
	position.x += (zoom_speed * front.x) * yoffset;
	position.y += (zoom_speed * front.y) * yoffset;
	position.z += (zoom_speed * front.z) * yoffset;
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

void remove_last_char(char* text) {
	unsigned int index = 0;
	while(1) {
		if(text[index] == '\0' && index > 0) {
			text[index - 1] = '\0';
			break;
		}

		index += 1;
	}
}

float lerp(float val_1, float val_2, float t) {
	if(t >= 1)
		return val_2;
	else if(t <= 0)
		return val_1;

	return (1 - t) * val_1 + t * val_2;
}

float distance(Vector3 *p1, Vector3 *p2) {
	float d = sqrt(
			(p1->x - p2->x) * (p1->x - p2->x) +
			(p1->y - p2->y) * (p1->y - p2->y) +
			(p1->z - p2->z) * (p1->z - p2->z)
			);

	return d;
}

int n_lerp(Vector3 *start, Vector3 *current, Vector3 *destination, float *vel, float acc, float dcc, float top_vel) {
	float current_d = distance(current, start);
	current_d += 0.5f;
	float total_distance = distance(destination, start);
	float decel_distance = total_distance / 2;

	if((total_distance - current_d) <= 0.05f)
		return 1;

	if((total_distance - current_d) >= 0.5f) {
		if(current_d <= decel_distance) {
			*vel = f_min(*vel + acc, top_vel);
		}
		else {
			*vel = f_max(*vel - dcc, 0);
			if(*vel == 0) {
				copy_vector(start, current);
				return 1;
			}
		}
	}

	return 0;
}
