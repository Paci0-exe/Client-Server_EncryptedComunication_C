#ifndef GESTORE_H
#define GESTORE_H

#include <stdio.h>

void spacchetta(char* buffer, int n);
int gestore(int client_sd, const char * s, int p);

extern char **blocchi;

#endif
