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
static Line axes[3];

/* Textures */
int violet_texture;
int gray_texture;
int red_texture;
int road_texture;
int junction_texture;
int signal_red_texture;
int signal_green_texture;
int palette_1_texture;
int palette_2_texture;
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
SignalCounter signal_counter;
/* Paths */

// Vector3 *all_pos = NULL;
// int *all_ids = NULL;
// Vector3 current_dir;
// int num_pos;
// int current_index;
// float current_angle;
// float fixed_way_point_distance;
// Model car;
// CAR_STATE car_state;


/* Models */
#define TOTAL_ROADS 258
#define ROADS_LIMIT 512
Model roads[TOTAL_ROADS];
Road _roads[ROADS_LIMIT];
unsigned int road_keys[ROADS_LIMIT];
unsigned int road_id_start, road_id_end;

#define TOTAL_JUNCTIONS 200
#define TOTAL_WORKING_JUNCTIONS 40
#define JUNCTIONS_LIMIT 256
Model junctions[TOTAL_JUNCTIONS];
Junction _junctions[JUNCTIONS_LIMIT];
unsigned int junction_keys[JUNCTIONS_LIMIT];
unsigned int junction_id_start, junction_id_end;

#define TOTAL_SIGNALS 160
#define SIGNALS_LIMIT 256
Model signals[TOTAL_SIGNALS];
Signal _signals[SIGNALS_LIMIT];
unsigned int signal_keys[SIGNALS_LIMIT];
unsigned int signal_id_start, signal_id_end;

#define CARS_LIMIT 256
Model _car_models[CARS_LIMIT];
Car _cars[CARS_LIMIT];
unsigned int car_keys[CARS_LIMIT];
unsigned int car_model_keys[CARS_LIMIT];
unsigned int current_car_ids[CARS_LIMIT];
unsigned int current_num_cars;
/* Models */

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
char* combine_string(const char* str_1, const char* str_2);
void character_callback(GLFWwindow* window, unsigned int codepoint);
void load_model(Model *model, const char* filename, int texture_id);
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
void move();
float get_angle(Vector3* point_1, Vector3* point_2, Vector3* default_direction);
void copy_data_to_models();
void update_all_signals(float time_passed);
void init_cars();
int is_colliding(int car_id, float distance_to_maintain);

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
		red_texture = make_texture(combine_string(assets_path, "png/red.png"));
		gray_texture = make_texture(combine_string(assets_path, "png/gray.png"));
		signal_red_texture = make_texture(combine_string(assets_path, "png/red_signal.png"));
		signal_green_texture = make_texture(combine_string(assets_path, "png/green_signal.png"));
		road_texture = make_texture(combine_string(assets_path, "png/road.png"));
		junction_texture = make_texture(combine_string(assets_path, "png/junction_gimp.png"));
		palette_1_texture = make_texture(combine_string(assets_path, "png/palette_1_gimp.png"));
		palette_2_texture = make_texture(combine_string(assets_path, "png/palette_2_gimp.png"));
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

	/* init view & projection */
	{
		make_identity(&view);
		projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
		text_projection = ortho(0, 1440.0f, 0, 900);
		init_vector(&front, -0.49f, -0.56f, -0.67f);
		init_vector(&position, -18.0f, 4.54f, 13);
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
	// load_model(&car, "car_1.model", palette_1_texture);
	// scale_model(&car, 0.4f, 0.4f, 0.4f);
	
	init_sim();
	/* Loading models */

	float fps = 0;
	float start = clock();
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		float before = clock();
		float now = (clock() - start) / CLOCKS_PER_SEC;

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		move();
		copy_data_to_models();
		update_all_signals(now);

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

		{
			for(int i = 0; i < current_num_cars; ++i) {
				Model *car = get_car_model(car_model_keys, _car_models, current_car_ids[i], CARS_LIMIT);
				draw_model(car, &view, &projection);
			}

			for(int i = 0; i < TOTAL_ROADS; ++i)
				draw_model(&roads[i], &view, &projection);
			for(int i = 0; i < TOTAL_JUNCTIONS; ++i)
				draw_model(&junctions[i], &view, &projection);
			for(int i = 0; i < TOTAL_SIGNALS; ++i)
				draw_model(&signals[i], &view, &projection);
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

			signal_1.color = RED;
			signal_2.color = RED;
			signal_3.color = RED;
			signal_4.color = RED;

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
			load_model(&roads[index], "road.model", road_texture);
			translate_model(&roads[index], start_x, start_y, start_z);
			rotate_model(&roads[index], 1, 0, 0, 90);
			scale_model(&roads[index], 2, 2, 1);

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
			load_model(&roads[index], "road.model", road_texture);
			translate_model(&roads[index], start_x, start_y, start_z);
			rotate_model(&roads[index], 1, 1, 1, 90);
			scale_model(&roads[index], 2, 2, 1);
			
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
			load_model(&junctions[index], "junction.model", junction_texture);
			translate_model(&junctions[index], start_x, start_y, start_z);
			rotate_model(&junctions[index], 1, 0, 0, 90);
			scale_model(&junctions[index], 2, 2, 1);
			index += 1;

			load_model(&junctions[index], "junction.model", junction_texture);
			translate_model(&junctions[index], start_x - 0.75f, start_y, start_z - 0.75f);
			rotate_model(&junctions[index], 1, 0, 0, 90);
			scale_model(&junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			load_model(&junctions[index], "junction.model", junction_texture);
			translate_model(&junctions[index], start_x + 0.75f, start_y, start_z - 0.75f);
			rotate_model(&junctions[index], 1, 0, 0, 90);
			scale_model(&junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			load_model(&junctions[index], "junction.model", junction_texture);
			translate_model(&junctions[index], start_x - 0.75f, start_y, start_z + 0.75f);
			rotate_model(&junctions[index], 1, 0, 0, 90);
			scale_model(&junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			load_model(&junctions[index], "junction.model", junction_texture);
			translate_model(&junctions[index], start_x + 0.75f, start_y, start_z + 0.75f);
			rotate_model(&junctions[index], 1, 0, 0, 90);
			scale_model(&junctions[index], 0.5f, 0.5f, 1);
			index += 1;

			load_model(&signals[signal_index], "signal_red.model", signal_red_texture);
			translate_model(&signals[signal_index], start_x - 0.75f, 0.5f, start_z - 0.75f);
			scale_model(&signals[signal_index], 0.25f, 0.25f, 0.25f);
			signal_index += 1;

			load_model(&signals[signal_index], "signal_red.model", signal_red_texture);
			translate_model(&signals[signal_index], start_x + 0.75f, 0.5f, start_z - 0.75f);
			scale_model(&signals[signal_index], 0.25f, 0.25f, 0.25f);
			rotate_model(&signals[signal_index], 0, 1, 0, 90);
			signal_index += 1;

			load_model(&signals[signal_index], "signal_red.model", signal_red_texture);
			translate_model(&signals[signal_index], start_x - 0.75f, 0.5f, start_z + 0.75f);
			scale_model(&signals[signal_index], 0.25f, 0.25f, 0.25f);
			rotate_model(&signals[signal_index], 0, 1, 0, -90);
			signal_index += 1;

			load_model(&signals[signal_index], "signal_red.model", signal_red_texture);
			translate_model(&signals[signal_index], start_x + 0.75f, 0.5f, start_z + 0.75f);
			scale_model(&signals[signal_index], 0.25f, 0.25f, 0.25f);
			rotate_model(&signals[signal_index], 0, 1, 0, 180);
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
		copy_vector(&road->position, &roads[i].position);
		current_id += 2;
	}
	for(int i = 0; i < TOTAL_WORKING_JUNCTIONS; ++i) {
		// @Note: Here the junction are placed the order: center, top_left, top_right, bottom_left, bottom_right
		Junction *junction = get_junction(junction_keys, _junctions, current_id, JUNCTIONS_LIMIT);
		copy_vector(&junction->position, &junctions[i*5].position); // @Note: i*5 because of the above note.
		current_id += 1;
	}
	for(int i = 0; i < TOTAL_SIGNALS; ++i) {
		Signal *signal = get_signal(signal_keys, _signals, current_id, SIGNALS_LIMIT);
		copy_vector(&signal->position, &signals[i].position);
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
	init_cars();
	/* Init Paths */
}

void init_paths() {
	char* p = combine_string(assets_path, "matrix/graph.matrix");
	Matrix *mat = load_matrix(p);
	convert_to_floyd_form(mat);
	paths = floyd_warshall(mat->mat, mat->len);

	signal_counter.should_start = 0;
	signal_counter.time_stamp = 0;
	signal_counter.signal_number = 1;
	signal_counter.time_interval = 2;

	// printf("road_id_start: %d, road_id_end: %d\n", road_id_start, road_id_end);
	// printf("junction_id_start: %d, junction_id_end: %d\n", junction_id_start, junction_id_end);
	// printf("signal_id_start: %d, signal_id_end: %d\n", signal_id_start, signal_id_end);
}

void init_cars() {
	const int total = 350;
	current_num_cars = 0;
	unsigned int current_car_id = 1;

	for(int i = 0; i < total; ++i) {
		current_car_ids[i] = current_car_id;
		int index = get(paths->keys, i * 2 + 1, i * 2 + 10, paths->limit);
		if(index == -1)
			continue;

		unsigned int len = paths->len[index] - 2;
		current_num_cars += 1;

		/* Making car */
		Car car;
		car.num_paths = len;
		car.path_pos = (Vector3*)malloc(sizeof(Vector3) * car.num_paths);
		car.path_ids = (int*)malloc(sizeof(int) * car.num_paths);
		car.car_state = START;
		car.current_index = -1;

		for(int i = 2; i < len + 2; ++i) {
			int node_id = paths->p[index][i];
			Vector3 pos = get_position_of_id(node_id);
			copy_vector(&car.path_pos[i - 2], &pos);
			car.path_ids[i - 2] = node_id;
		}
		put_car(car_keys, _cars, current_car_id, &car, CARS_LIMIT);

		/* Making model */
		Model car_model;
		if((i + 1) % 2 == 0)
			load_model(&car_model, "car_1.model", palette_2_texture);
		else
			load_model(&car_model, "car_1.model", palette_1_texture);

		scale_model(&car_model, 0.4f, 0.4f, 0.4f);
		put_car_model(car_model_keys, _car_models, current_car_id, &car_model, CARS_LIMIT);

		current_car_id += 1;
	}
	printf("current_num_cars: %d\n", current_num_cars);
}

int is_colliding(int car_id, float distance_to_maintain) {
	Car *test_car = get_car(car_keys, _cars, car_id, CARS_LIMIT);

	for(int i = 0; i < current_num_cars; ++i) {
		unsigned int current_id = current_car_ids[i];
		if(current_id == car_id)
			continue;

		Car *car = get_car(car_keys, _cars, current_id, CARS_LIMIT);
		if(car->current_angle == test_car->current_angle) {
			if((test_car->current_angle == 180 && test_car->position.z > car->position.z) || 
				(test_car->current_angle == 0 && test_car->position.z < car->position.z) || 
				// @Hack: for some reason current_angle == 90.0f fails.
				(test_car->current_angle >= 89 && test_car->current_angle <= 91 && test_car->position.x > car->position.x) || 
				(test_car->current_angle == 270 && test_car->position.x < car->position.x)) {
				float distance = get_distance(&test_car->position, &car->position);
				if(distance < distance_to_maintain)
					return 1;
			}
		}
	}

	return 0;
}

void move() {
	for(int i = 0; i < current_num_cars; ++i) {
		unsigned int current_id = current_car_ids[i];
		Car *car = get_car(car_keys, _cars, current_id, CARS_LIMIT);

		if(car->car_state == START) {
			init_vector(&car->position, car->path_pos[0].x, car->path_pos[0].y + 0.25f, car->path_pos[0].z);
			init_vector(&car->rotation_axes, 0, 1, 0);
			car->car_state = WAY_POINT;
			car->current_index = 0;
			car->current_angle = 180;
		}
		else if(car->car_state == WAY_POINT) {
			if(car->current_index == car->num_paths - 1) {
				car->car_state = STOP;	
				return;
			}
			if(is_junction(car->path_ids[car->current_index + 1])) {
				int junction_id = car->path_ids[car->current_index + 1];
				Junction *junction = get_junction(junction_keys, _junctions, junction_id, JUNCTIONS_LIMIT);
				Signal *signal = NULL;
				if(car->current_angle == 0)
					signal = get_signal(signal_keys, _signals, junction->to_down_right_signal_id, SIGNALS_LIMIT);
				else if(car->current_angle == 180)
					signal = get_signal(signal_keys, _signals, junction->to_top_left_signal_id, SIGNALS_LIMIT);
				else if(car->current_angle == 90)
					signal = get_signal(signal_keys, _signals, junction->to_down_left_signal_id, SIGNALS_LIMIT);
				else
					signal = get_signal(signal_keys, _signals, junction->to_top_right_signal_id, SIGNALS_LIMIT);

				if(signal->color == RED)
					continue;
			}
			Vector3* from = &car->path_pos[car->current_index];
			Vector3* to = &car->path_pos[car->current_index + 1];
			car->fixed_way_point_distance = get_distance(from, to);
			car->current_direction = sub(to, from);
			normalize_vector(&car->current_direction);
			Vector3 car_default_direction;
			init_vector(&car_default_direction, 0, 0, 1);
			float angle = get_angle(from, to, &car_default_direction);
			car->current_angle = angle;
			init_vector(&car->rotation_axes, 0, 1, 0);
			car->current_index += 1;
			car->car_state = MOVING;
		}
		else if(car->car_state == MOVING) {
			if(is_colliding(current_id, 0.9f))
				continue;

			Vector3 tmp_pos = scalar_mul(&car->current_direction, 0.03f);
			car->position = add(&car->position, &tmp_pos);

			Vector3 from, to;
			copy_vector(&from, &car->position);
			copy_vector(&to, &car->path_pos[car->current_index - 1]);
			from.y = 0;
			to.y = 0;
			float distance = get_distance(&from, &to);
			if(distance >= car->fixed_way_point_distance) {
				car->car_state = WAY_POINT;
			}
		}
		else if(car->car_state == STOP) {
		}
	}
}

// @Note: Switches lights of signals every 'signal_counter.time_interval' seconds, clockwise.
void update_all_signals(float time_passed) {
	if(signal_counter.should_start == 0) {
		signal_counter.should_start = 1;
		signal_counter.time_stamp = time_passed;
	}
	else {
		if((time_passed - signal_counter.time_stamp) >= signal_counter.time_interval) {
			signal_counter.time_stamp = time_passed;
			unsigned int start_junction_id = 1 + (2 * (TOTAL_ROADS - 1)) + 2;
			for(int i = 0; i < TOTAL_WORKING_JUNCTIONS; ++i) {
				Junction *junction = get_junction(junction_keys, _junctions, start_junction_id, JUNCTIONS_LIMIT);
				Signal *signal_tl = get_signal(signal_keys, _signals, junction->to_top_left_signal_id, SIGNALS_LIMIT);
				Signal *signal_tr = get_signal(signal_keys, _signals, junction->to_top_right_signal_id, SIGNALS_LIMIT);
				Signal *signal_bl = get_signal(signal_keys, _signals, junction->to_down_left_signal_id, SIGNALS_LIMIT);
				Signal *signal_br = get_signal(signal_keys, _signals, junction->to_down_right_signal_id, SIGNALS_LIMIT);
				signal_tl->color = RED;
				signal_tr->color = RED;
				signal_bl->color = RED;
				signal_br->color = RED;
				start_junction_id += 1;

				// @Note: Clockwise.
				if(signal_counter.signal_number == 1)
					signal_tl->color = GREEN;
				else if(signal_counter.signal_number == 2)
					signal_tr->color = GREEN;
				else if(signal_counter.signal_number == 3)
					signal_br->color = GREEN;
				else if(signal_counter.signal_number == 4)
					signal_bl->color = GREEN;
			}

			signal_counter.signal_number += 1;
			if(signal_counter.signal_number > 4)
				signal_counter.signal_number = 1;
		}
	}
}

void copy_data_to_models() {
	/* SIGNALS */
	unsigned int start_signal_id = signal_id_start;

	for(int i = 0; i < TOTAL_SIGNALS; ++i) {
		Signal *signal = get_signal(signal_keys, _signals, start_signal_id, SIGNALS_LIMIT);
		if(signal->color == RED)
			signals[i].texture_id = signal_red_texture;
		else
			signals[i].texture_id = signal_green_texture;

		start_signal_id += 1;
	}
	/* SIGNALS */

	/* CARS */
	for(int i = 0; i < current_num_cars; ++i) {
		unsigned int current_id = current_car_ids[i];
		Car *car = get_car(car_keys, _cars, current_id, CARS_LIMIT);
		Model *model = get_car_model(car_model_keys, _car_models, current_id, CARS_LIMIT);

		translate_model(model, car->position.x, car->position.y, car->position.z);
		rotate_model(model, car->rotation_axes.x, car->rotation_axes.y, car->rotation_axes.z, car->current_angle);
	}
	/* CARS */
}

// @Note: This does not do what you think. This does not give correct angle for all the cases.
float get_angle(Vector3* point_1, Vector3* point_2, Vector3* default_direction) {
	Vector3 tmp_1;
	tmp_1.x = point_1->x + default_direction->x;
	tmp_1.y = point_1->y + default_direction->y;
	tmp_1.z = point_1->z + default_direction->z;

	Vector3 vec1 = sub(&tmp_1, point_1);
	Vector3 vec2 = sub(point_2, point_1);
	
	float x1 = vec1.x;
	float x2 = vec2.x;
	float z1 = vec1.z;
	float z2 = vec2.z;
	if(x1 == x2) {
		if(z2 < z1)
			return 180;
		else
			return 0;
	}
	else if(z1 == z2) {
		if(x1 > x2)
			return 90;
		else
			return 270;
	}
	else {
		float numerator = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
		float denominator = sqrt(vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z) * sqrt(vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z);
		float angle = acos(numerator / denominator) * (180 / 3.1415926f);
		
		if((x1 < x2 && z1 > z2) || (x1 < x2 && z1 < z2))
			return 360 - angle;
		return angle;
	}
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
			if(id % 2 == 0)
				pos.x += 0.3f;
			else
				pos.x -= 0.3f;
		}
		else {
			if(id % 2 == 0)
				pos.z += 0.3f;
			else
				pos.z -= 0.3f;
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

void load_model(Model *model, const char* filename, int texture_id) {
	FILE *file = NULL;
	const char* path = "../../data/models/";
	char final_path[50];
	strcpy(final_path, path);
	strcat(final_path, filename);
	file = fopen(final_path, "r");

	if(file == NULL) {
		printf("Could not open file %s\n", final_path);
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

		make_model(model, shader1, texture_id, vertices, total_floats, total_vertices, &local_origin, width, height, depth);

		free(vertices);
		fclose(file);
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
