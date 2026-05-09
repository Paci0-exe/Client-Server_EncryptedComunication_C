#include <stdio.h>
#include <stdlib.h>

#include "fileUtils.h"

unsigned char *read_all_file(const char *filename, int *length) {
    FILE *fp = open_file(filename, "rb");

    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    rewind(fp);

    unsigned char *content = malloc((*length) + 1); // +1 per terminatore \0
    if (!content) {
        perror("Errore di allocazione memoria");
        close_file(fp);
        return NULL;
    }

    fread(content, 1, *length, fp);
    //content[*length] = '\0';

    close_file(fp);
    return content;
}


int write_into_file(const char *filename, const char *mode, const unsigned char *data, int data_len) {

	FILE *fp = open_file(filename, mode);
	
	// Scrive la stringa nel file
	fwrite(data, sizeof(unsigned char), data_len, fp);	

    close_file(fp);
    return 0;
}

FILE *open_file(const char *filename, const char *mode) {
	FILE *fp = fopen(filename, mode);
	if (!fp) {
		perror("Errore nell'apertura del file per scrittura");
		exit(1);
	}
	return fp;
}

void close_file(FILE *fp) {
	if (fclose(fp) == EOF) {
		perror("Errore durante la chiusura del file");
		exit(1);
	}
}
