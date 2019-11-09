#include "utils.h"
#include <stdio.h>

int read_file(const char* filename, char *buffer) {
	FILE *file = NULL;
	file = fopen(filename, "r");
	if(file == NULL)
		return -1;

	fseek(file, 0, SEEK_END);
	const size_t filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fread(buffer, 1, filesize, file);
	buffer[filesize] = 0;
	
	fclose(file);
	return 0;
}
