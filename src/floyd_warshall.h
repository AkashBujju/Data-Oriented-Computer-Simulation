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
	char **keys;
	int num_paths;
};
typedef struct Paths Paths;

Paths* floydWarshall(int** graph, int len);
Paths* get_solution(int** cost, int** path, int len);
int combine_ints(int num_1, int num_2);
int get_closest_power_of_2(int number);
int get(char **keys, int src_node, int des_node, int limit);
int put(int src_node, int des_node, char **keys, int **paths, int *path, int limit);
unsigned int get_hash(char *str, int len, int limit);
int* get_path(int** path, int v, int u, int* des_len);
char* get_str(int num_1, int num_2);
void convert_to_floyd_form(Matrix *mat);
void set_matrix(Matrix *matrix, int len);
void convert_to_int(char** str, int* res, int len);
Matrix* load_matrix(const char* filename);
char** split(char* src, const char* delim, int *des_len);


#endif
