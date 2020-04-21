#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

struct Matrix {
	int** mat;
	int len;
};
typedef struct Matrix Matrix;

struct Paths {
	int **p;
	int *len;
	int *keys;
	int num_paths;
	int limit;
};
typedef struct Paths Paths;

void init_matrix_floyd(Matrix* matrix, int len);
void convert_to_int(char** str, int* res, int len);
void convert_to_floyd_form(Matrix* mat);
Matrix* load_matrix(const char* filename);
char** split(char* src, const char* delim, int* des_len);
Paths* floyd_warshall(int** graph, int len);
int* get_path(int** path, int v, int u, int* des_len);
Paths* get_solution(int** cost, int** path, int len);
void print_paths(Paths* paths);
int combine_ints(int num_1, int num_2);
int get(int* keys, int src_node, int des_node, int limit);
int put(int src_node, int des_node, int* keys, int** paths, int* path, int limit);
unsigned int get_hash(char* str, int len, int limit);
char* get_str(int num_1, int num_2);
int get_closest_power_of_2(int number);
int get_unique_number(int num1, int num2);
void print_matrix_floyd(Matrix* matrix);

#endif
