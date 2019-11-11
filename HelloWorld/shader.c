#include <glad/glad.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "shader.h"
#include "utils.h"
#include "math.h"
#include "stb_image.h"

int compile_shader(const char* vertex_file, const char* fragment_file) {
	int success;
	char info_log[512];

	const size_t sz = 8192;
	char *tmp1 = (char*)calloc(sz, sizeof(char));
	const GLchar* vertex_str = (GLchar*)tmp1;
	if(read_file(vertex_file, tmp1) != 0) {
		printf("File %s not found.\n", vertex_file);
		return -1;
	}
	char *tmp2 = (char*)calloc(sz, sizeof(char));
	const GLchar* fragment_str = (GLchar*)tmp2;
	if(read_file(fragment_file, tmp2) != 0) {
		printf("File %s not found.\n", fragment_file);
		return -1;
	}

	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_str, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		printf("VertexShader error: %s\n", info_log);
	}

	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_str, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		printf("FragmentShader error: %s\n", info_log);
	}

	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		  printf("ShaderLink error: %s\n", info_log);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

	return shader_program;
}

int make_texture(const char* filename) {
	int texture;

	stbi_set_flip_vertically_on_load(1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		printf("Failed to load texture %s\n", filename);
	}
	stbi_image_free(data);
	
	return texture;
}

void set_matrix4(unsigned int program, const char* name, Matrix4 *mat) {
	glUseProgram(program);
	unsigned int transform_loc = glGetUniformLocation(program, name);
	glUniformMatrix4fv(transform_loc, 1, GL_FALSE, mat->matrix);
}

void set_vector3(unsigned int program, const char* name, struct Vector3 *vec) {
	glUseProgram(program);
	unsigned int transform_loc = glGetUniformLocation(program, name);
	glUniform3f(transform_loc, vec->x, vec->y, vec->z);
}
