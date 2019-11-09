#ifndef SHADER_H
#define SHADER_H

struct Matrix4;

int compile_shader(const char* vertex_file, const char* fragment_file);
void set_matrix4(unsigned int program, const char* name, struct Matrix4 *mat);

#endif
