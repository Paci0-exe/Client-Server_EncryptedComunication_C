#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <stdlib.h>

unsigned char *read_all_file(const char*, int*);
int write_into_file(const char*, const char*, const unsigned char*, int);
FILE *open_file(const char*, const char*);
void close_file(FILE*);

#endif
