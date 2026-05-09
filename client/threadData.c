#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "threadData.h"
#include "crypt.h"
#include "blockManager.h"

ThreadData* newThreadData(unsigned char* input, unsigned char* key) {

	ThreadData *d = malloc(sizeof(ThreadData));
	
	d -> input = input;
	d -> key = key;
	d -> output = NULL;
	
	return d;
}


ThreadData **create_td_list(unsigned char *string, int str_len, unsigned char* key, bool padding, int *n_blocks) {
	unsigned char **blocks = get_blocks(string, str_len, padding, n_blocks);
	
	ThreadData **td_list = malloc(*n_blocks * sizeof(ThreadData*));
	for (int i = 0; i < *n_blocks; i++) {
		td_list[i] = newThreadData(blocks[i], key);
	}

	return td_list;
}



void *crypt_string_p(void *arg) {
	ThreadData *d = (ThreadData*)arg;
	unsigned char *output = crypt_string(d -> input, d -> key);
	d -> output = output;
	return 0;
}


unsigned char *handlePThreads(unsigned char *string, int str_len, unsigned char *key, int p, bool padding, int *mess_len) {
	//printf("p: %d\n", p);
	
	// Creo la lista di ThreadData e salvo
	// il numero di blocchi
	int n_blocks = 0;
	ThreadData **td_list = create_td_list(string, str_len, key, padding, &n_blocks);
	
	// Alloco memoria per p thread
	pthread_t *threads = malloc(sizeof(pthread_t) * p);
	if (!threads) {
		perror("[Client] Errore di allocazione");
		exit(1);
	}	

	
	
	int j = 0;
	int busy_threads = 0; 
	// Il while continua finchè non abbiamo gestito
	// tutti i blocchi creati
	printf("[Client] Blocco dei segnali in corso...\n");
	clock_t start = clock();
	
	// Blocco dei segnali
	sigset_t *mask = block_signals();
	while (j < n_blocks) {
		// Iniziamo con la creazione di più thread possibili
		// utilizziamo i%p per riutilizzare threads già liberati
		for (int i = 0; i < n_blocks; i++){ 
			
			int thread_creation = pthread_create(&threads[i%p], NULL, crypt_string_p, td_list[i]);
			if (thread_creation != 0) {
				perror("[Client] Errore durante la creazione del thread");
				exit(1);
			}
			
			//printf("Thread[%d] creato con stringa \"%s\"\n", i%p, td_list[i]->input);
			
			// Aumento il contatore di thread busy
			busy_threads++;
			
			
			// Una volta che i thread sono pieni (= p) o
			// che abbiamo finito i blocchi allora inizia
			// attendiamo che i threads terminino l'esecuzione
			if (busy_threads >= p || ++j == n_blocks) {
				for (int k = 0; k < busy_threads; k++) pthread_join(threads[k], NULL);
				busy_threads = 0;
			}
		}
	}
	printf("[Client] Sblocco dei segnali in corso...\n");
	// Sblocco dei segnali
	unblock_signals(mask);
	clock_t end = clock();
	
	printf("[Client] Tempo impiegato per crypt decrypt: %f secondi \n", ((double)(end - start)) / CLOCKS_PER_SEC);
	return compose_message(td_list, n_blocks, mess_len);	
}


unsigned char *compose_message(ThreadData** td_list, int n_blocks, int *mess_len) {

	int BLOCK_SIZE = get_block_size();
	int size = (n_blocks * BLOCK_SIZE) + 1;
	unsigned char *message = malloc(sizeof(unsigned char) * size);
	message[0] = '\0';
	if (message == NULL) {
		perror("[Client] Errore durante l'allocazione di memoria");
		exit(1);
	}

	
	*mess_len = 0;
	
	for (int i = 0; i < n_blocks; i++) {
		unsigned char *output = td_list[i] -> output;
		for (int j = 0; j < BLOCK_SIZE; j++) {
			message[(*mess_len)++] = output[j];
		}
	}
	
	return message;
}


/*
 * La funzione blocca i seguenti segnali:
 * SIGUSR1
 * SIGUSR2
 * SIGALRM
 * SIGTERM
 * SIGINT
 */
sigset_t *block_signals() {
	sigset_t *mask = malloc(sizeof(sigset_t));
	
	sigemptyset(mask);
	sigaddset(mask, SIGUSR1);
	sigaddset(mask, SIGUSR2);
	sigaddset(mask, SIGALRM);
	sigaddset(mask, SIGINT);
	sigaddset(mask, SIGTERM);
	
	pthread_sigmask(SIG_BLOCK, mask, NULL);
	return mask;
}

/*
 * La funzione sblocca eventuali segnali
 * bloccati presenti nella variabile mask
 * passata
 */
void unblock_signals(sigset_t *mask) {
	pthread_sigmask(SIG_UNBLOCK, mask, NULL);
	free(mask);
}
