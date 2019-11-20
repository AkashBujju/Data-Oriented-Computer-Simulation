#ifndef SHADER_H
#define SHADER_H

struct Matrix4;
struct Vector3;

unsigned int compile_shader(const char* vertex_file, const char* fragment_file);
unsigned int make_texture(const char* filename);
void set_matrix4(unsigned int program, const char* name, struct Matrix4 *mat);
void set_vector3(unsigned int program, const char* name, struct Vector3 *vec);
void set_float(unsigned int program, const char* name, float f);

#endif
