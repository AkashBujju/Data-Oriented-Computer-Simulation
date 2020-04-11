#include "floyd_warshall.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>

Paths* floydWarshall(int** graph, int len) {
	int** dist = (int**)malloc(sizeof(int*) * len);
	int** path = (int**)malloc(sizeof(int*) * len);
	int i, j, k;

	for(int i = 0; i < len; i += 4) {
		dist[i] = (int*)malloc(sizeof(int) * len);
		dist[i + 1] = (int*)malloc(sizeof(int) * len);
		dist[i + 2] = (int*)malloc(sizeof(int) * len);
		dist[i + 3] = (int*)malloc(sizeof(int) * len);

		path[i] = (int*)malloc(sizeof(int) * len);
		path[i + 1] = (int*)malloc(sizeof(int) * len);
		path[i + 2] = (int*)malloc(sizeof(int) * len);
		path[i + 3] = (int*)malloc(sizeof(int) * len);
	}

	for (i = 0; i < len; i++) {
		for (j = 0; j < len; ++j) {
			dist[i][j] = graph[i][j]; 

			if(i == j)
				path[i][j] = 0;
			else if(dist[i][j] < 999)
				path[i][j] = i;
			else
				path[i][j] = -1;
		}
	}

	for(k = 0; k < len; k++) {
		for(i = 0; i < len; i++) {
			int dist_i_k = dist[i][k];

			for(j = 0; j < len; j += 4) {
				if(dist_i_k != 999 && dist[k][j] != 999 &&
						dist_i_k + dist[k][j] < dist[i][j]) {
					dist[i][j] = dist_i_k + dist[k][j]; 
					path[i][j] = path[k][j];
				}
				if(dist_i_k != 999 && dist[k][j + 1] != 999 &&
						dist_i_k + dist[k][j + 1] < dist[i][j + 1]) {
					dist[i][j + 1] = dist_i_k + dist[k][j + 1]; 
					path[i][j + 1] = path[k][j + 1];
				}
				if(dist_i_k != 999 && dist[k][j + 2] != 999 &&
						dist_i_k + dist[k][j + 2] < dist[i][j + 2]) {
					dist[i][j + 2] = dist_i_k + dist[k][j + 2]; 
					path[i][j + 2] = path[k][j + 2];
				}
				if(dist_i_k != 999 && dist[k][j + 3] != 999 &&
						dist_i_k + dist[k][j + 3] < dist[i][j + 3]) {
					dist[i][j + 3] = dist_i_k + dist[k][j + 3]; 
					path[i][j + 3] = path[k][j + 3];
				}
			} 
		} 
	} 

	Paths* paths = get_solution(dist, path, len);
	return paths;
} 

Paths* get_solution(int** cost, int** path, int len) {
	int total = combine_ints(len, len - 1);
	int limit = get_closest_power_of_2(total);

	Paths *paths = (Paths*)malloc(sizeof(Paths));
	paths->num_paths = limit;
	paths->p = (int**)malloc(sizeof(int*) * limit);
	paths->len = (int*)malloc(sizeof(int) * limit);
	paths->keys = (char**)malloc(sizeof(char*) * limit);

	for(int i = 0; i < limit; i += 4) {
		paths->p[i] = NULL;
		paths->p[i + 1] = NULL;
		paths->p[i + 2] = NULL;
		paths->p[i + 3] = NULL;

		paths->len[i] = 0;
		paths->len[i + 1] = 0;
		paths->len[i + 2] = 0;
		paths->len[i + 3] = 0;

		paths->keys[i] = (char*)malloc(sizeof(char) * 2);
		paths->keys[i + 1] = (char*)malloc(sizeof(char) * 2);
		paths->keys[i + 2] = (char*)malloc(sizeof(char) * 2);
		paths->keys[i + 3] = (char*)malloc(sizeof(char) * 2);

		strcpy(paths->keys[i], "0\0");
		strcpy(paths->keys[i + 1], "0\0");
		strcpy(paths->keys[i + 2], "0\0");
		strcpy(paths->keys[i + 3], "0\0");
	}

	for (int v = 0; v < len; v++) {
		for (int u = 0; u < len; u++) {
			if (u != v && path[v][u] != -1) {
				int l;
				int *value = get_path(path, v, u, &l);
				int gen_index = put(v, u, paths->keys, paths->p, value, limit);
				paths->len[gen_index] = l;
			}
		}
	}

	return paths;
}

int combine_ints(int num_1, int num_2) {
	char a[15], b[15], c[30];

	_itoa(num_1, a, 10);
	_itoa(num_2, b, 10);

	int index_1 = 0, index_2 = 0;
	while(a[index_1] != '\0')
		c[index_2++] = a[index_1++];

	index_1 = 0;
	while(b[index_1] != '\0')
		c[index_2++] = b[index_1++];
	c[index_2] = '\0';

	return atoi(c);
}

int get_closest_power_of_2(int number) {
	int result = 0;
	while(pow(2, result) <= number)
		result++;

	return (int)pow(2, result);
}

unsigned int get_hash(char *str, int len, int limit) {
	unsigned int hash = 5381;
	int c;

	while(c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash % limit;
}

int get(char **keys, int src_node, int des_node, int limit) {
	char* key = get_str(src_node, des_node);
	int len = strlen(key);
	unsigned int i = get_hash(key, len, limit);
	char* result_key = keys[i];

	while(strcmp(result_key, "0") != 0) {
		if(strcmp(result_key, key) == 0)
			return i;

		i = (i + 1) & (limit - 1);
		result_key = keys[i];
	}

	return -1;
}

int put(int src_node, int des_node, char **keys, int **paths, int *path, int limit) {
	char* key = get_str(src_node, des_node);
	int len = strlen(key);
	unsigned int i = get_hash(key, len, limit);
	char* result_key = keys[i];

	while(strcmp(result_key, "0") != 0) {
		if(strcmp(result_key, key) == 0)
			break;

		i = (i + 1) & (limit - 1);
		result_key = keys[i];
	}

	keys[i] = key;
	paths[i] = path;

	return i;
}

int* get_path(int** path, int v, int u, int* des_len) {
	int des[100];
	int len = 0;

	int first = v;
	int last = u;

	while(path[v][u] != v) {
		des[len++] = path[v][u];
		u = path[v][u];
	}

	*des_len = len + 4;
	int* des_arr = (int*)malloc(sizeof(int) * (*des_len));
	des_arr[0] = first;
	des_arr[1] = last;
	des_arr[2] = first;
	des_arr[*des_len - 1] = last;

	int index = 3;
	for(int i = len - 1; i >= 0; --i)
		des_arr[index++] = des[i];

	return des_arr;
}

// @Note: Give a better function name
char* get_str(int num_1, int num_2) {
	char a[15], b[15];
	char* c = (char*)malloc(sizeof(char) * 15);

	_itoa(num_1, a, 10);
	_itoa(num_2, b, 10);

	int index_1 = 0, index_2 = 0;
	while(a[index_1] != '\0')
		c[index_2++] = a[index_1++];

	index_1 = 0;
	while(b[index_1] != '\0')
		c[index_2++] = b[index_1++];
	c[index_2] = '\0';

	return c;
}

void convert_to_floyd_form(Matrix *mat) {
	for(int i = 0; i < mat->len; ++i) {
		for(int j = 0; j < mat->len; ++j) {
			if(i != j && mat->mat[i][j] == 0)
				mat->mat[i][j] = 999;
		}
	}
}

void set_matrix(Matrix *matrix, int len) {
	matrix->mat = (int**)malloc(sizeof(int*) * len);
	matrix->len = len;

	for(int i = 0; i < len; ++i)
		matrix->mat[i] = (int*)malloc(sizeof(int) * len);
}

void convert_to_int(char** str, int* res, int len) {
	for(int i = 0; i < len; ++i)
		res[i] = atoi(str[i]);
}

Matrix* load_matrix(const char* filename) {
	FILE *file = NULL;
	fopen_s(&file, filename, "r");

	fseek(file, 0, SEEK_END); 
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* str = (char*)malloc(sizeof(char) * (size + 1));
	fread(str, 1, size, file);
	str[size] = '\0';
	fclose(file);

	int len;
	char** split_str = split(str, "\n", &len);
	Matrix *matrix = (Matrix*)malloc(sizeof(Matrix) * len);
	set_matrix(matrix, len);

	for(int i = 0; i < len; ++i) {
		int l;
		char** tmp_numbers = split(split_str[i], ", ", &l);
		convert_to_int(tmp_numbers, matrix->mat[i], l);
	}

	free(str);
	return matrix;
}

char** split(char* src, const char* delim, int *des_len) {
	char *next = NULL, *token = NULL;
	char **str = (char**)malloc(sizeof(char*) * 100);
	unsigned int index = 0;

	token = strtok_s(src, delim, &next);
	if(token != NULL)
		str[index++] = token;

	while((token = strtok_s(NULL, delim, &next)) != NULL) {
		str[index++] = token;
	}

	*des_len = index;

	return str;
}

