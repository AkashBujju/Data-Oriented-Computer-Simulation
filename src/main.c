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
#include "cube_uv.h"
#include "rectangle.h"
#include "custom_rect.h"
#include "grid.h"
#include "line.h"
#include "cube.h"
#include "model.h"
#include "text.h"

enum CoPos {
	B_L, T_R,
	T_L, B_R
};
typedef enum CoPos CoPos;

unsigned int window_width = 400;
unsigned int window_height = 400;
Matrix4 view, projection, text_projection;
Vector3 position, up, front;
int firstMouse = 1;
float yaw = -90.0f;
float pitch = -40.0f;
float lastX, lastY;
int shader1, shader2, text_shader;
int show_cursor;

Line lines[500];
int lines_count;
Cuboid points[100];
int points_count;
static Line texture_lines[500];
static Rectangle rectangles[100];
static CustomRect custom_rects[100];
static CustomRect custom_texture_rects[100];
static Cuboid texture_points[100];
static int indices[100];
static int bounding_indices[8];
static int index_count = 0;
static int texture_indices[100];
static int texture_index_count = 0;
static int texture_lines_count;
static int rectangles_count;
static int custom_rect_count;
static int custom_texture_rect_count;
static int texture_points_count;
static CoPos co_pos[4];
static int current_co_pos_index;
static CoPos co_pos_texture[4];
static int current_texture_co_pos_index;
static int current_bounding_index;
static int current_rect_index;
static int current_texture_rect_index;

static Font font;
static Cuboid origin;
static Line axes[3];
static Grid grid;
static Rectangle texture_rect;
static Model *model;

/* Modes */
static int place_mode = 1;
static int join_mode = 0;
static int toggle_grid = 1;
static int texture_mode = 0;
static int model_mode = 1;
static int bounding_mode = 0;
static int pick_mode = 0;
static int texture_pick_mode = 0;
static int rect_pick_mode = 1;
/* Modes */

/* Display Text */
static char input_text[100];
static int input_mode = 0;
static float grid_trans_delta = 0.5f;
static char texture_rect_filename[50];
static Vector3 filter_axes;
static int show_origin = 1;
static float zoom_speed = 1.0f;
/* Display Text */

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void add_line(Vector3 start, Vector3 end, float r, float g, float b, Line *lines_arr, int *current_total);
void add_rectangle(Vector3* position, Vector3* scale, Vector3* rotation_axes, float angle_in_degree, const char* image, int shader);
void add_custom_rect(CustomRect *custom_rects, int *count, Vector3 *p1, Vector3 *p2, Vector3 *p3, Vector3 *p4, int program, int texture_id, Vector2 *t_p1, Vector2 *t_p2, Vector2 *t_p3, Vector2 *t_p4);
void add_point(float x, float y, float z, float scale, Cuboid* points_arr, int *current_total);
char* combine_string(const char* str_1, const char* str_2);
void update_custom_rect(int rect_index_to_update, int texture_rect_index);
void character_callback(GLFWwindow* window, unsigned int codepoint);
void save_vertices(const char* filename);
void load_model(const char* filename);
void apply_texture();
void remove_last_char(char* text);
void process_command(char* command);
void append_char(char* str, char c);

const char* assets_path = "../../data/";
const char* shaders_path = "../../shaders/";
const char* profs_path = "../../profs/";

int red_texture, red_texture_border;
int blue_texture_border;
int gray_texture_border;
int violet_texture;
int mixed_texture;
int bordered_transparent_rect_texture;

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
		red_texture = make_texture(combine_string(assets_path, "png/red.png"));
		red_texture_border = make_texture(combine_string(assets_path, "png/rectangle_red.png"));
		blue_texture_border = make_texture(combine_string(assets_path, "png/rectangle_blue.png"));
		gray_texture_border = make_texture(combine_string(assets_path, "png/rectangle_gray.png"));
		violet_texture = make_texture(combine_string(assets_path, "png/safety_blue.png"));
		mixed_texture = make_texture(combine_string(assets_path, "png/test.png"));
		bordered_transparent_rect_texture = make_texture(combine_string(assets_path, "png/bordered_transparent_rect.png"));
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

		strcpy(texture_rect_filename, "png/test.png");
		make_rectangle(&texture_rect, shader1, combine_string(assets_path, texture_rect_filename));
		scale_rectangle(&texture_rect, 6, 6, 1);
		translate_rectangle(&texture_rect, 30, 0, 0);

		make_grid(&grid, 100, 100, 0.2f, 0.2f);
		translate_grid(&grid, 0, 0, 0);
		// rotate_grid(&grid, 1, 0, 0, 90);

		model = NULL;
	}

	/* init view & projection */
	{
		make_identity(&view);
		projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
		text_projection = ortho(0, 1440.0f, 0, 900);
		init_vector(&front, -0.49f, -0.56f, -0.67f);
		init_vector(&position, 36.77f, 42.26, 44.36f);
		init_vector(&up, 0, 1, 0);
	}

	/* Init other variables */
	{
		show_cursor = 1;
		lines_count = 0;
		texture_lines_count = 0;
		rectangles_count = 0;
		points_count = 0;
		co_pos[0] = B_L; co_pos[1] = B_L; co_pos[2] = B_L; co_pos[3] = B_L;
		co_pos_texture[0] = B_L; co_pos_texture[1] = B_L; co_pos_texture[2] = B_L; co_pos_texture[3] = B_L;
		current_co_pos_index = -1;
		current_texture_co_pos_index = -1;
		current_bounding_index = -1;
		custom_rect_count = 0;
		custom_texture_rect_count = 0;
		current_rect_index = -1;
		current_texture_rect_index = -1;

		init_vector(&filter_axes, 0, 0, 1);
	}

	/* Init font */
	FT_Library ft;
	init_freetype(&ft);
	init_font(&font, combine_string(assets_path, "fonts/consolas.ttf"), &ft);
	/* Init font */

	float fps = 0;
	// float start_time = clock();
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		float before = clock();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		/* Grid Delta */
		{
			char grid_delta_str[20];
			char tmp[10];
			gcvt(grid_trans_delta, 4, tmp);
			
			strcpy(grid_delta_str, "grid_delta: ");
			strcat(grid_delta_str, tmp);
			render_text(&font, text_shader, grid_delta_str, 0.02f * window_width, 0.15f * window_height, 1, 0, 1, 0);
		}

		/* FPS */
		{
			if(fps >= 55 && fps <= 65)
				fps = 60;

			char fps_str[20];
			char tmp[10];
			gcvt(fps, 2, tmp);
			
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

		/* Filter axes */
		{
			char camera_position_str[50];
			char tmp[10];

			sprintf(tmp, "%d", (int)filter_axes.x);
			strcpy(camera_position_str, "filter_axes: ");
			strcat(camera_position_str, tmp);

			sprintf(tmp, "%d", (int)filter_axes.y);
			strcat(camera_position_str, ", ");
			strcat(camera_position_str, tmp);

			sprintf(tmp, "%d", (int)filter_axes.z);
			strcat(camera_position_str, ", ");
			strcat(camera_position_str, tmp);
			render_text(&font, text_shader, camera_position_str, 0.02f * window_width, 0.80f * window_height, 1, 0, 1, 0);
		}

		/* Zoom Speed */
		{
			char zoom_speed_str[20];
			char tmp[10];
			gcvt(zoom_speed, 4, tmp);
			
			strcpy(zoom_speed_str, "zoom_speed: ");
			strcat(zoom_speed_str, tmp);
			render_text(&font, text_shader, zoom_speed_str, 0.02f * window_width, 0.75f * window_height, 1, 0, 1, 0);
		}

		draw_rectangle(&texture_rect, &view, &projection);
		if(show_origin) {
			draw_cuboid(&origin, &view, &projection);
			draw_line(&axes[0], &view, &projection);
			draw_line(&axes[1], &view, &projection);
			draw_line(&axes[2], &view, &projection);
		}

		if(toggle_grid) {
			draw_grid(&grid, &view, &projection);
		}

		if(model != NULL) {
			draw_model(model, &view, &projection);
		}

		for(int i = 0; i < texture_points_count; ++i)
			draw_cuboid(&texture_points[i], &view, &projection);
		for(int i = 0; i < lines_count; ++i)
			draw_line(&lines[i], &view, &projection);
		for(int i = 0; i < texture_lines_count; ++i)
			draw_line(&texture_lines[i], &view, &projection);
		for(int i = 0; i < rectangles_count; ++i)
			draw_rectangle(&rectangles[i], &view, &projection);
		for(int i = 0; i < custom_rect_count; ++i)
			draw_custom_rect(&custom_rects[i], &view, &projection);
		for(int i = 0; i < custom_texture_rect_count; ++i)
			draw_custom_rect(&custom_texture_rects[i], &view, &projection);
		for(int i = 0; i < points_count; ++i)
			draw_cuboid(&points[i], &view, &projection);

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
	printf("Lines count: %d\n", lines_count);
	return 0;
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
	else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		translate_grid_by(&grid, 0, 0, grid_trans_delta);
	}
	else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		translate_grid_by(&grid, 0, 0, -grid_trans_delta);
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
		place_mode = 1;
		join_mode = 0;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		place_mode = 0;
		join_mode = 1;
	}
	else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		toggle_grid = !toggle_grid;
	}
	else if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		texture_mode = 0;
		pick_mode = 0;
		bounding_mode = 0;
		model_mode = 1;
	}
	else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		model_mode = 0;
		pick_mode = 0;
		bounding_mode = 0;
		texture_mode = 1;
	}
	else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		model_mode = 0;
		pick_mode = 0;
		texture_mode = 0;
		bounding_mode = 1;
	}
	else if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		model_mode = 0;
		texture_mode = 0;
		bounding_mode = 0;
		pick_mode = 1;
	}
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		texture_pick_mode = 0;
		rect_pick_mode = 1;
	}
	else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		texture_pick_mode = 1;
		rect_pick_mode = 0;
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		apply_texture();
	}
	else if(key == GLFW_KEY_1 && action == GLFW_PRESS) {
		if(model_mode) {
			current_co_pos_index += 1;
			co_pos[current_co_pos_index] = 0;
		}
		else if(texture_mode) {
			current_texture_co_pos_index += 1;
			co_pos_texture[current_texture_co_pos_index] = 0;
		}
		else if(bounding_mode) {
			current_bounding_index = 0;
		}
	}
	else if(key == GLFW_KEY_2 && action == GLFW_PRESS) {
		if(model_mode) {
			current_co_pos_index += 1;
			co_pos[current_co_pos_index] = 1;
		}
		else if(texture_mode) {
			current_texture_co_pos_index += 1;
			co_pos_texture[current_texture_co_pos_index] = 1;
		}
		else if(bounding_mode) {
			current_bounding_index = 1;
		}
	}
	else if(key == GLFW_KEY_3 && action == GLFW_PRESS) {
		if(model_mode) {
			current_co_pos_index += 1;
			co_pos[current_co_pos_index] = 2;
		}
		else if(texture_mode) {
			current_texture_co_pos_index += 1;
			co_pos_texture[current_texture_co_pos_index] = 2;
		}
		else if(bounding_mode) {
			current_bounding_index = 2;
		}
	}
	else if(key == GLFW_KEY_4 && action == GLFW_PRESS) {
		if(model_mode) {
			current_co_pos_index += 1;
			co_pos[current_co_pos_index] = 3;
		}
		else if(texture_mode) {
			current_texture_co_pos_index += 1;
			co_pos_texture[current_texture_co_pos_index] = 3;
		}
		else if(bounding_mode) {
			current_bounding_index = 3;
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

	if(strcmp(split_str[0], "load") == 0) {
		load_model(split_str[1]);
	}
	else if(strcmp(split_str[0], "save") == 0) {
		save_vertices(split_str[1]);
	}
	else if(strcmp(split_str[0], "grid_delta") == 0) {
		grid_trans_delta = strtof(split_str[1], NULL);
	}
	else if(strcmp(split_str[0], "texture_filename") == 0) {
		strcpy(texture_rect_filename, split_str[1]);
		make_rectangle(&texture_rect, shader1, combine_string(assets_path, texture_rect_filename));
		scale_rectangle(&texture_rect, 6, 6, 1);
		translate_rectangle(&texture_rect, 30, 0, 0);
	}
	else if(strcmp(split_str[0], "filter_axes") == 0) {
		if(strcmp(split_str[1], "x") == 0) {
			init_vector(&filter_axes, 1, 0, 0);
		}
		else if(strcmp(split_str[1], "y") == 0) {
			init_vector(&filter_axes, 0, 1, 0);
		}
		else if(strcmp(split_str[1], "z") == 0) {
			init_vector(&filter_axes, 0, 0, 1);
		}
	}
	else if(strcmp(split_str[0], "toggle") == 0) {
		if(strcmp(split_str[1], "origin") == 0) {
			show_origin = !show_origin;
		}
	}
	else if(strcmp(split_str[0], "zoom_speed") == 0) {
		zoom_speed = strtof(split_str[1], NULL);
	}
}

void apply_texture() {
	printf("applying texture\n");

	update_custom_rect(current_rect_index, current_texture_rect_index);
}

void save_vertices(const char* filename) {
	FILE *file = NULL;
	const char* path = "../../data/models/";
	char final_path[50];
	strcpy(final_path, path);
	strcat(final_path, filename);
	file = fopen(final_path, "w");

	if(file == NULL) {
		printf("Could not open file %s\n", final_path);
	}
	else {
		int total_floats = 30 * custom_rect_count;
		int num_vertices = 6 * custom_rect_count;

		fprintf(file, "total_floats %d\n", total_floats);
		fprintf(file, "num_vertices %d\n", num_vertices);
		fprintf(file, "texture_file_name %s\n", combine_string(assets_path, texture_rect_filename));

		for(int i = 0; i < custom_rect_count; ++i) {
			CustomRect *c = &custom_rects[i];

			fprintf(file, "%.2f,%.2f,%.2f,%.2f,%.2f ",  c->ll.x, c->ll.y, c->ll.z, c->t_ll.x, c->t_ll.y);
			fprintf(file, "%.2f,%.2f,%.2f,%.2f,%.2f ",  c->ul.x, c->ul.y, c->ul.z, c->t_ul.x, c->t_ul.y);
			fprintf(file, "%.2f,%.2f,%.2f,%.2f,%.2f\n", c->ur.x, c->ur.y, c->ur.z, c->t_ur.x, c->t_ur.y);
			fprintf(file, "%.2f,%.2f,%.2f,%.2f,%.2f ",  c->ll.x, c->ll.y, c->ll.z, c->t_ll.x, c->t_ll.y);
			fprintf(file, "%.2f,%.2f,%.2f,%.2f,%.2f ",  c->lr.x, c->lr.y, c->lr.z, c->t_lr.x, c->t_lr.y);
			fprintf(file, "%.2f,%.2f,%.2f,%.2f,%.2f\n", c->ur.x, c->ur.y, c->ur.z, c->t_ur.x, c->t_ur.y);
		}

		fprintf(file, "bounding_box\n");
		Cuboid *b1 = &points[bounding_indices[0]];
		Cuboid *b2 = &points[bounding_indices[1]];
		Cuboid *b3 = &points[bounding_indices[2]];
		Cuboid *b4 = &points[bounding_indices[3]];

		float width = sqrt(
			(b1->position.x - b2->position.x) * (b1->position.x - b2->position.x) +
			(b1->position.y - b2->position.y) * (b1->position.y - b2->position.y) +
			(b1->position.z - b2->position.z) * (b1->position.z - b2->position.z)
		);

		float height = sqrt(
			(b1->position.x - b3->position.x) * (b1->position.x - b3->position.x) +
			(b1->position.y - b3->position.y) * (b1->position.y - b3->position.y) +
			(b1->position.z - b3->position.z) * (b1->position.z - b3->position.z)
		);

		float depth = sqrt(
			(b1->position.x - b4->position.x) * (b1->position.x - b4->position.x) +
			(b1->position.y - b4->position.y) * (b1->position.y - b4->position.y) +
			(b1->position.z - b4->position.z) * (b1->position.z - b4->position.z)
		);

		fprintf(file, "local_origin %.2f,%.2f,%.2f\n", b1->position.x + width/2, b1->position.y - height/2, b1->position.z - depth/2);
		fprintf(file, "width %.2f height %.2f depth %.2f\n",  width, height, depth);

		fclose(file);
		printf("SAVED\n");
	}
}

void load_model(const char* filename) {
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

		char texture_filename[50];
		fscanf(file, "%s", tmp);
		fscanf(file, "%s", texture_filename);

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

		model = (Model*)malloc(sizeof(Model));
		make_model(model, shader1, vertices, total_floats, total_vertices, texture_filename, &local_origin, width, height, depth);

		printf("\nmodel_loaded\n");
		free(vertices);
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

		if(pick_mode) {
			if(texture_pick_mode) {
				Vector3 pos;
				int result_index = i_custom_rects(custom_texture_rects, custom_texture_rect_count, &ray, &pos, &filter_axes);
				if(result_index != -1) {
					current_texture_rect_index = result_index;
					printf("current_texture_rect_index: %d\n", current_texture_rect_index);
				}
			}
			else if(rect_pick_mode) {
				Vector3 pos;
				int result_index = i_custom_rects(custom_rects, custom_rect_count, &ray, &pos, &filter_axes);
				if(result_index != -1) {
					current_rect_index = result_index;
					printf("current_rect_index: %d\n", current_rect_index);
				}
			}
		}
		else if(bounding_mode) {
			if(join_mode) {
				int result_index = obb(points, points_count, &ray);
				if(result_index >= 0) {
					if(current_bounding_index == -1) {
						printf("Warning: current_bounding_index = -1\n");
					}
					else {
						bounding_indices[current_bounding_index] = result_index;
					}
				}
			}
		}
		else if(texture_mode) {
			if(place_mode) {
				Vector3 pos_on_rect;
				int res = i_quad(&texture_rect, &ray, &pos_on_rect);
				if(res) {
					pos_on_rect.z += 0.1f;
					add_point(pos_on_rect.x, pos_on_rect.y, pos_on_rect.z, 0.1f, texture_points, &texture_points_count);
				}
			}
			else if(join_mode) {
				int result_index = obb(texture_points, texture_points_count, &ray);
				if(result_index >= 0) {
					texture_index_count++;
					int index = texture_index_count - 1;
					int t = 4 * floor(index / 4);

					if(current_texture_co_pos_index == -1) {
						printf("Warning: current_texture_co_pos_index = -1\n");
					}
					else {
						texture_indices[t + co_pos_texture[current_texture_co_pos_index]] = result_index;
						if(current_texture_co_pos_index > 0) {
							Cuboid *from_cuboid = &texture_points[texture_indices[t + co_pos_texture[current_texture_co_pos_index - 1]]];
							Cuboid *to_cuboid = &texture_points[texture_indices[t + co_pos_texture[current_texture_co_pos_index]]];
							add_line(from_cuboid->position, to_cuboid->position, 1, 1, 1, texture_lines, &texture_lines_count);
							if(texture_index_count % 4 == 0) {
								Vector3 *p1 = &texture_points[texture_indices[t + co_pos_texture[current_texture_co_pos_index - 3]]].position;
								add_line(*p1, to_cuboid->position, 1, 1, 1, texture_lines, &texture_lines_count);
								current_texture_co_pos_index = -1;

								Vector3 *ll = &texture_points[texture_indices[t + 0]].position;
								Vector3 *ul = &texture_points[texture_indices[t + 1]].position;
								Vector3 *ur = &texture_points[texture_indices[t + 2]].position;
								Vector3 *lr = &texture_points[texture_indices[t + 3]].position;

								Vector2 t_p1, t_p2, t_p3, t_p4;
								init_vector2(&t_p1, 0, 0);
								init_vector2(&t_p2, 0, 1);
								init_vector2(&t_p3, 1, 1);
								init_vector2(&t_p4, 1, 0);

								add_custom_rect(custom_texture_rects, &custom_texture_rect_count, ll, ul, ur, lr, shader1, bordered_transparent_rect_texture, &t_p1, &t_p2, &t_p3, &t_p4);
								current_texture_rect_index = custom_texture_rect_count - 1;
							}

							from_cuboid->texture_id = red_texture;
							to_cuboid->texture_id = blue_texture_border;
						}
					}
				}
			}
		}
		else if(model_mode) {
			if(place_mode) {
				/* Testing grid */
				Vector3 plane_point;
				int hit_plane = in_plane_point(&grid.box, &plane_point, &from, &to);

				if(hit_plane) {
					Vector3 grid_hit_point;
					int hit_grid = get_sub_grid_mid_point(&grid, &plane_point, &grid_hit_point);
					if(hit_grid) {
						add_point(grid_hit_point.x, grid_hit_point.y, grid_hit_point.z, 0.02f, points, &points_count);
					}
				}
				/* Testing grid */
			}
			else if(join_mode) {
				int result_index = obb(points, points_count, &ray);
				if(result_index >= 0) {
					index_count++;
					int index = index_count - 1;
					int t = 4 * floor(index / 4);

					if(current_co_pos_index == -1) {
						printf("Warning: current_co_pos_index = -1\n");
					}
					else {
						indices[t + co_pos[current_co_pos_index]] = result_index;
						if(current_co_pos_index > 0) {
							Cuboid *from_cuboid = &points[indices[t + co_pos[current_co_pos_index - 1]]];
							Cuboid *to_cuboid = &points[indices[t + co_pos[current_co_pos_index]]];
							add_line(from_cuboid->position, to_cuboid->position, 1, 0, 0, lines, &lines_count);
							if(index_count % 4 == 0) {
								Vector3 *p1 = &points[indices[t + co_pos[current_co_pos_index - 3]]].position;
								add_line(*p1, to_cuboid->position, 1, 1, 1, lines, &lines_count);
								current_co_pos_index = -1;

								Vector3 *ll = &points[indices[t + 0]].position;
								Vector3 *ul = &points[indices[t + 1]].position;
								Vector3 *ur = &points[indices[t + 2]].position;
								Vector3 *lr = &points[indices[t + 3]].position;

								Vector2 t_p1, t_p2, t_p3, t_p4;
								init_vector2(&t_p1, 0, 0);
								init_vector2(&t_p2, 0, 1);
								init_vector2(&t_p3, 1, 1);
								init_vector2(&t_p4, 1, 0);

								add_custom_rect(custom_rects, &custom_rect_count, ll, ul, ur, lr, shader1, bordered_transparent_rect_texture, &t_p1, &t_p2, &t_p3, &t_p4);
								current_rect_index = custom_rect_count - 1;
							}

							from_cuboid->texture_id = red_texture;
							to_cuboid->texture_id = blue_texture_border;
						}
					}
				}
			}
		}
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

void add_line(Vector3 start, Vector3 end, float r, float g, float b, Line *lines_arr, int *current_total) {
	*current_total += 1;
	if(*current_total > 500) {
		printf("No more lines to use!!!!!!!!!!\n");
		return;
	}

	int index = *current_total - 1;
	make_line(&lines_arr[index], &start, &end, shader2);
	init_vector(&lines_arr[index].color, r, g, b);
}

void update_custom_rect(int rect_index_to_update, int texture_rect_index) {
	if(rect_index_to_update < 0 || texture_rect_index < 0) {
		printf("rect_index_to_update < 0 || texture_rect_index < 0\n");
		return;
	}

	CustomRect *c = &custom_rects[rect_index_to_update];
	CustomRect *t = &custom_texture_rects[texture_rect_index];

	Vector2 p[4];
	init_vector2(&p[0], t->ll.x, t->ll.y);
	init_vector2(&p[1], t->ul.x, t->ul.y);
	init_vector2(&p[2], t->ur.x, t->ur.y);
	init_vector2(&p[3], t->lr.x, t->lr.y);

	Vector3 tmp;
	float width_by_2 = texture_rect.width / 2;
	float height_by_2 = texture_rect.height / 2;
	for(int i = 0; i < 4; ++i) {
		init_vector(&tmp, p[i].x, p[i].y, 0);
		Vector3 relative_point = sub(&tmp, &texture_rect.position);
		relative_point.x = f_normalize(relative_point.x, -width_by_2, width_by_2, 0, 1);
		relative_point.y = f_normalize(relative_point.y, -height_by_2, height_by_2, 0, 1);

		p[i].x = relative_point.x;
		p[i].y = relative_point.y;
	}

	make_custom_rect(&custom_rects[rect_index_to_update], &c->ll, &c->ul, &c->ur, &c->lr, shader1, texture_rect.texture_id, &p[0], &p[1], &p[2], &p[3]);
}

void add_custom_rect(CustomRect *custom_rect, int *count, Vector3 *p1, Vector3 *p2, Vector3 *p3, Vector3 *p4, int program, int texture_id, Vector2 *t_p1, Vector2 *t_p2, Vector2 *t_p3, Vector2 *t_p4) {
	*count += 1;
	if(*count > 100) {
		printf("No more custom_rects to use!!!!!!!!!!\n");
	}
	else {
		int index = *count - 1;
		make_custom_rect(&custom_rect[index], p1, p2, p3, p4, program , texture_id, t_p1, t_p2, t_p3, t_p4);
	}
}

void add_rectangle(Vector3* position, Vector3* scale, Vector3* rotation_axes, float angle_in_degree, const char* image, int shader) {
	rectangles_count += 1;
	if(rectangles_count > 100) {
		printf("No more rectangles to use!!!!!!!!!!\n");
	}
	else {
		int index = rectangles_count - 1;
		make_rectangle(&rectangles[index], shader, image);
		translate_rectangle(&rectangles[index], position->x, position->y, position->z);
		rotate_rectangle(&rectangles[index], rotation_axes->x, rotation_axes->y, rotation_axes->z, angle_in_degree);
		scale_rectangle(&rectangles[index], scale->x, scale->y, scale->z);
	}
}

void add_point(float x, float y, float z, float scale, Cuboid* points_arr, int *current_total) {
	*current_total += 1;
	if(*current_total > 100) {
		printf("No more points to use!!!!!!!!!!\n");
		return;
	}

	int index = *current_total - 1;
	make_cuboid(&points_arr[index], shader1, red_texture_border);
	scale_cuboid(&points_arr[index], scale, scale, scale);
	translate_cuboid(&points_arr[index], x, y, z);
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
