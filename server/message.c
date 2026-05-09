#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * Crea un messaggio nella forma: messaggio + \0 + lunghezza_come_stringa + \0 + chiave
 */
unsigned char *createMessage(unsigned char *mess, int mess_len, unsigned char* key, int key_len, int* tot_len){

    char m_len[sizeof(int) * 3 + 1]; // Buffer per contenere il numero come stringa
    sprintf(m_len, "%d", mess_len);

    // Calcola la lunghezza totale: mess + \0 + len_mess + \0 + key
    int total_length = mess_len + 1 + strlen(m_len) + 1 + key_len;
    unsigned char *result = (unsigned char*)malloc(total_length);

    if (result == NULL) {
        perror("[Server] Errore allocazione memoria in createMessage");
        *tot_len = 0;
        return NULL;
    }

    int pos = 0;
    
    // Copia il messaggio (che può contenere \0 all'interno)
    memcpy(result + pos, mess, mess_len);
    pos += mess_len;
    result[pos++] = '\0';  // Separatore

    // Copia la lunghezza del messaggio come stringa
    memcpy(result + pos, m_len, strlen(m_len));
    pos += strlen(m_len);
    result[pos++] = '\0';  // Separatore

    // Copia la chiave
    memcpy(result + pos, key, key_len);
    pos += key_len;

    // Imposta la lunghezza totale
    *tot_len = total_length;

    printf("[Server] Messaggio creato - Lunghezza totale: %d\n", total_length);
    printf("[Server] Lunghezza messaggio: %s\n", m_len);
    printf("[Server] Lunghezza chiave: %d\n", key_len);

    return result;
}

/*
 * Legge un messaggio considerando che il messaggio può contenere byte \0 all'interno
 * Struttura: [messaggio_binario][lunghezza_bytes][separatore \0][lunghezza_stringa][separatore \0][chiave]
 */
message *readMessage(unsigned char* string) {
    
    message *m = malloc(sizeof(message));
    if (m == NULL) {
        perror("[Server] Errore allocazione memoria in readMessage");
        return NULL;
    }
    
    // APPROCCIO ALTERNATIVO: Prima trova la chiave (che è alla fine)
    // poi usa la lunghezza per determinare dove inizia il messaggio
    
    // Trova l'ultimo \0 (prima della chiave)
    int total_received = 1024; // Assumiamo che sia la dimensione del buffer
    int last_null = -1;
    int second_last_null = -1;
    
    // Trova gli ultimi due \0
    for (int i = total_received - 1; i >= 0; i--) {
        if (string[i] == '\0') {
            if (last_null == -1) {
                last_null = i;
            } else if (second_last_null == -1) {
                second_last_null = i;
                break;
            }
        }
    }
    
    if (second_last_null == -1 || last_null == -1) {
        printf("[Server] Errore: formato messaggio non valido\n");
        free(m);
        return NULL;
    }
    
    // La chiave è dopo l'ultimo \0
    m->key = string + last_null + 1;
    
    // La lunghezza è tra i due \0
    char *length_str = (char*)(string + second_last_null + 1);
    m->length = atoi(length_str);
    
    // Il messaggio inizia dall'inizio ed è lungo m->length bytes
    m->mess = string;
    
    printf("[Server] Messaggio letto (NUOVO APPROCCIO):\n");
    printf("[Server] Lunghezza: %d\n", m->length);
    printf("[Server] Chiave: %s\n", m->key);
    printf("[Server] Posizione secondo_ultimo_null: %d\n", second_last_null);
    printf("[Server] Posizione ultimo_null: %d\n", last_null);
    
    return m;
}

/*
 * VERSIONE ALTERNATIVA - Usa lunghezza fissa per il parsing
 * Questa versione presuppone di conoscere la lunghezza totale del messaggio ricevuto
 */
message *readMessageWithLength(unsigned char* string, int total_len) {
    
    message *m = malloc(sizeof(message));
    if (m == NULL) {
        perror("[Server] Errore allocazione memoria in readMessage");
        return NULL;
    }
    
    // Trova gli ultimi due \0 partendo dalla fine
    int last_null = -1;
    int second_last_null = -1;
    
    for (int i = total_len - 1; i >= 0; i--) {
        if (string[i] == '\0') {
            if (last_null == -1) {
                last_null = i;
            } else if (second_last_null == -1) {
                second_last_null = i;
                break;
            }
        }
    }
    
    if (second_last_null == -1 || last_null == -1) {
        printf("[Server] Errore: formato messaggio non valido (lunghezze: %d, %d)\n", second_last_null, last_null);
        free(m);
        return NULL;
    }
    
    // La chiave è dopo l'ultimo \0 fino alla fine
    m->key = string + last_null + 1;
    
    // La lunghezza è tra i due \0
    string[last_null] = '\0'; // Termina temporaneamente la stringa della lunghezza
    char *length_str = (char*)(string + second_last_null + 1);
    m->length = atoi(length_str);
    string[last_null] = '\0'; // Ripristina (era già \0 comunque)
    
    // Il messaggio è dall'inizio per m->length bytes
    m->mess = string;
    
    printf("[Server] Messaggio letto con lunghezza %d:\n", total_len);
    printf("[Server] Lunghezza messaggio: %d\n", m->length);
    printf("[Server] Chiave: %s\n", m->key);
    printf("[Server] Separatori trovati alle posizioni: %d, %d\n", second_last_null, last_null);
    
    return m;
}
