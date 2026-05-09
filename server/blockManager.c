#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blockManager.h"


#define BLOCK_SIZE 8

/*
 * Data una stringa esegue il padding
 * aggiungendo \0 sse la stringa non è
 * divisibile per 8
*/
unsigned char *parse_string(unsigned char *string, int str_len) {

	
	// Se è divisibile per BLOCK_SIZE non c'è bisogno di parsing
	if (str_len % 8 == 0) return string;
	
	// Altrimenti mi calcolo la sua nuova lunghezza divisibile per 8
	int new_str_len = ((int)(str_len / BLOCK_SIZE) + 1) * BLOCK_SIZE;
	
	// Alloco memoria al risultato e ne scrivo in memoria
	// la stringa con parsing
	unsigned char* result = (unsigned char*)calloc(new_str_len + 1, sizeof(unsigned char));
	if (!result) {
		perror("Errore di allocazione");
		exit(1);
	}
	memcpy(result, string, str_len);
	 
	return result;

}

/*
 * Suddivide la stringa in blocchi lunghi BLOCK_SIZE.
 * Notare che nell'intero passato alla funzione viene salvato
 * il numero di blocchi.
 */
unsigned char **get_blocks(unsigned char *input, int input_len, bool parsed, int *num_of_blocks) {
	
	// Eseguo il parsed della stringa se richiesto
	if (parsed) input = parse_string(input, input_len);
	
	// Ricavo la lunghezza della stringa
	int str_len = input_len;
	
	// Mi calcolo il numero di blocchi
	// e lo salvo nell'intero passato
	*num_of_blocks = (str_len % BLOCK_SIZE == 0) ? str_len / BLOCK_SIZE : (str_len / BLOCK_SIZE) + 1;
	
	// Alloco memoria al puntatore di blocchi 
	unsigned char **blocks = malloc(sizeof(unsigned char*) * *num_of_blocks);
	if (!blocks) {
		perror("Errore di allocazione");
		exit(1);
	}   
	
	// Alloco memoria ad ogni singolo blocco
	// ed inserisco una porzione di stringa per ogni blocco
	for (int i = 0; i < *num_of_blocks; i++) {
		blocks[i] = malloc(BLOCK_SIZE * sizeof(unsigned char));
		if (!blocks[i]) {
			perror("Errore di allocazione");
			exit(1);
		}
		
		memcpy(blocks[i], &input[i * BLOCK_SIZE], BLOCK_SIZE); 
		//blocks[i][BLOCK_SIZE] = '\0';
	}
	return blocks;
	
}

int get_block_size() {
	return BLOCK_SIZE;
}

