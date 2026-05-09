#include "crypt.h"
#include "threadData.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define KEY_LENGTH 8

/*
 * Crypta una stringa passata utilizzando
 * la chiave key
 */
unsigned char* crypt_string(unsigned char *string, unsigned char *key) {

	unsigned char *crypted_string = malloc(KEY_LENGTH * sizeof(unsigned char));
	for(int i = 0; i < KEY_LENGTH; i++) {
		crypted_string[i] = string[i] ^ key[i];
	}
	return crypted_string; 
}

/*
 * Esegue il decrypt di una stringa cyptata
 * dalla funzione crypt_string tramite la stessa
 * chiave key.
*/
unsigned char *decrypt_string(unsigned char *string, unsigned char *key) {
	return crypt_string(string, key);
}


int get_key_length() {
	return KEY_LENGTH;
}

