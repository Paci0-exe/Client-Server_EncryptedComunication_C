#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inputHandler.h"

/*
La struct rappresenta la versione
"lunga" dei parametri, se hanno bisogno
di argomenti opzionali/obbligatori, e qual è
la loro versione 'ridotta'
*/
struct option long_options[] = {
	{"port", required_argument, NULL, 'd'},
	{"connection", required_argument, NULL, 'i'},
	{"file", required_argument, NULL, 'f'},
	{"parallelism", required_argument, NULL, 'p'},
	{0, 0, 0, 0}
};

/*
Rappresenta la versione ridotta dei parametri
dove nome_parametro: significa che il parametro
necessita di un input
*/
const char *short_options = "d:i:f:p:";

/*
Crea e ritorna un nuovo handler in base
ai parametri passati dallo stdin.
*/
handler *newHandler(int argc, char *argv[]) {
	handler *h = malloc(sizeof(handler));
	
	// Inizializzo tramite variabili d'ambiente
	h -> file_name = getenv("FILE_NAME");
	h -> port = getenv("SERVER_PORT");
	h -> connection = getenv("MAX_CONNECTION");
	h -> parallelism = getenv("PARALLELISM");
	
	//printf("Variabili d'ambiente impostate \n");
	
	// Aggiorno con eventuali parametri passati da terminale	
	handleParams(argc, argv, h);
	return h;
}


/*
Gestisce i vari parametri passati andando
ad aggiornare l'handler.
*/
void handleParams(int argc, char *argv[], handler *h) {
	int opt;
	while((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {	
		switch (opt) {
			
			case 'f':
				h -> file_name = optarg;
				break;
			case 'i':
				h -> connection = optarg;
				break;
			case 'd':
				h -> port = optarg;
				break; 	
			case 'p':
				h -> parallelism = optarg;
				break;
			default:
				fprintf(stderr, "Errore: parametro inesistente: %c\n",opt); exit(1);	
		}
	}
	//printf("Variabili in bash impostate\n");
	
	if (!h->file_name || !h->port || !h->connection || !h->parallelism) {
		fprintf(stderr, "Errore: parametri in input mancanti o errati\n");
		exit(1);
	}
}
