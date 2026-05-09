#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H


#include <stdbool.h>

unsigned char *pars_string(unsigned char*, int);
unsigned char** get_blocks(unsigned char*, int, bool, int*);
int get_block_size();



#endif
