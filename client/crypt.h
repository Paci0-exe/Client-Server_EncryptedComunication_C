#ifndef CRYPT_H
#define CRYPT_H

#include "threadData.h"

unsigned char* crypt_string(unsigned char*, unsigned char*);
unsigned char* decrypt_string(unsigned char*, unsigned char*);
int get_key_length();
#endif
