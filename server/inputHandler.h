#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>

// Definisce la struttura di un handler
typedef struct {
	char* file_name;
	char* connection;
	char* port;
	char* parallelism;
} handler;


// Ritorna un nuovo handler
handler *newHandler(int argc, char *argv[]);


// Gestisce i vari parametri passati
// dallo stdin
void handleParams(int, char**, handler*);

#endif
