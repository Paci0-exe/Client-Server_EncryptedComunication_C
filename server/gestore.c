#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "crypt.h"
#include "message.h"
#include "threadData.h"

#define BUFSIZE 1024
#define BLOCK_SIZE 8

char **blocchi;

// Dichiarazione della funzione alternativa (da aggiungere in message.h)
message *readMessageWithLength(unsigned char* string, int total_len);

int gestore(int client_sd, const char* s, int p){

    int letto, scritti;
    char buffer[BUFSIZE];
    message *m;
    char *ACK = "ACK";
    int num_block;
    
    printf("[Server] Gestore avviato, attendo messaggio...\n");
    
    memset(buffer, 0, BUFSIZE);
    letto = read(client_sd, buffer, BUFSIZE);
    
    printf("[Server] Read completata, bytes letti: %d\n", letto);
    
    if (letto < 0) {
        perror("[Server] Errore in lettura");
        return -1;
    }
    
    if (letto == 0) {
        printf("[Server] Connessione chiusa dal client\n");
        return -1;
    }
    
    printf("[Server] Dati ricevuti (%d bytes), parsing messaggio...\n", letto);
    
    // Debug: stampa i bytes ricevuti in formato hex e char
    printf("[Server] Dati ricevuti (hex e char):\n");
    for (int i = 0; i < letto && i < 60; i++) {
        printf("[%02d]: 0x%02X ", i, (unsigned char)buffer[i]);
        if (buffer[i] == '\0') {
            printf("(\\0)");
        } else if (buffer[i] >= 32 && buffer[i] <= 126) {
            printf("('%c')", buffer[i]);
        } else {
            printf("(.)");
        }
        printf("\n");
    }
    
    // Usa la versione che accetta la lunghezza
    m = readMessageWithLength((unsigned char*)buffer, letto);
    if (m == NULL) {
        printf("[Server] Errore parsing messaggio\n");
        return -1;
    }
    
    printf("[Server] Messaggio parsato con successo\n");
    
    unsigned char *message = (unsigned char*)m->mess;
    printf("[Server] Messaggio (primi 20 bytes): ");
    for (int i = 0; i < 20 && i < m->length; i++) {
        printf("0x%02X ", message[i]);
    }
    printf("\n");
    
    char *k =(char *) m->key;
    printf("[Server] Chiave: %s (lunghezza: %d)\n", k, (int)strlen(k));
    int l = m->length;
    printf("[Server] Lunghezza messaggio: %d\n", l);
    
    if (l <= 0) {
        printf("[Server] ERRORE: Lunghezza messaggio non valida: %d\n", l);
        return -1;
    }
    
    printf("[Server] Inizio decrittazione...\n");
    char *decrittato = (char*)handlePThreads(message, l, (unsigned char*)k, p, false, &num_block);
    
    if (decrittato == NULL) {
        printf("[Server] Errore durante la decrittazione\n");
        return -1;
    }
    
    printf("[Server] Decrittazione completata (%d blocchi)\n", num_block);
    
    // Debug: mostra il risultato decrittato
    printf("[Server] Testo decrittato (primi 50 caratteri): ");
    for (int i = 0; i < 50 && decrittato[i] != '\0'; i++) {
        if (decrittato[i] >= 32 && decrittato[i] <= 126) {
            printf("%c", decrittato[i]);
        } else {
            printf(".");
        }
    }
    printf("\n");
    
    // Invio ACK
    printf("[Server] Invio ACK...\n");
    scritti = write(client_sd, ACK, strlen(ACK));
    if (scritti < 0) {
        perror("[Server] Errore invio ACK");
        return -1;
    }
    
    printf("[Server] ACK inviato (%d bytes)\n", scritti);
    
    // Creazione file di output
    printf("[Server] Scrittura file di output: %s\n", s);
    FILE *outputFile = fopen(s, "w");
    if (!outputFile) {
        fprintf(stderr, "[Server] Errore nell'apertura del file di output: %s\n", s);
        return -1;
    }

    // Scrivi file decrittato - usa lunghezza per evitare problemi con \0 interni
    size_t written = fwrite(decrittato, 1, strlen(decrittato), outputFile);
    fclose(outputFile);
    
    printf("[Server] File scritto con successo (%zu bytes)\n", written);
    
    // Pulizia memoria
    free(m);
    free(decrittato);
    
    return 0;
}
