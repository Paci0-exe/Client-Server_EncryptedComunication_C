#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "client.h"
#include "inputHandler.h"
#include "blockManager.h"
#include "threadData.h"
#include "fileUtils.h"
#include "crypt.h"
#include "message.h"

void print_byte(unsigned char *bytes, int len) {
	for (int i = 0; i < len; i++)
		printf("%c", bytes[i]);
	printf("\n");
}

int main(int arg, char* argv[]) {

    handler *h = newHandler(arg, argv);
    char *file_name = h->file_name;
    unsigned char *key = (unsigned char*)h->key;
    int p = atoi(h->parallelism);
    
    int mess_len = 0;
    unsigned char *file_content = read_all_file(file_name, &mess_len);
    
    int final_mess_len = 0;
    unsigned char *final_message = handlePThreads(file_content, mess_len, key, p, true, &final_mess_len);
    
    const char* OUTPUT_FILE = "crypted_message.txt";
    write_into_file(OUTPUT_FILE, "wb", final_message, mess_len);
    
    int c_socket = createSocket();
    if (c_socket == -1) {
        printf("[Client] Errore creazione socket\n");
        return -1;
    }
        
    char *server_addr = h->address;
    int server_port = atoi(h->port);
    
    int conn_res = connect_to_server(c_socket, server_addr, server_port);
    if (conn_res == -1) {
        printf("[Client] Errore connessione al server\n");
        return -1;
    }
    
    printf("[Client] Connesso al server %s:%d\n", server_addr, server_port);
    
    int key_len = get_key_length();
    int total_mess_len = 0;  // CORREZIONE: variabile per lunghezza totale messaggio
    unsigned char *u_mess = createMessage(final_message, final_mess_len, key, key_len, &total_mess_len);
    
    if (u_mess == NULL || total_mess_len == 0) {
        printf("[Client] Errore creazione messaggio\n");
        close(c_socket);
        return -1;
    }
    
    printf("[Client] Invio messaggio di %d bytes...\n", total_mess_len);
    
    char* mess = (char*)u_mess;
    int sent = send_message(c_socket, mess, total_mess_len, 0);  // CORREZIONE: usa total_mess_len
    
    if (sent <= 0) {
        printf("[Client] Errore invio messaggio\n");
        close(c_socket);
        return -1;
    }
    
    printf("[Client] Messaggio inviato (%d bytes), attendo risposta...\n", sent);
    
    int resp_len = 1024;
    char response[resp_len];
    int received = recv_message(c_socket, response, resp_len-1, 0);
    
    if (received > 0) {
        response[received] = '\0';  // Assicura terminazione stringa
        printf("[Client] Risposta ricevuta: %s\n", response);
    } else {
        printf("[Client] Errore ricezione risposta\n");
    }

    close(c_socket);
    printf("[Client] Connessione chiusa\n");
    
    // Pulizia memoria
    free(u_mess);
    
    return 0;
}

int createSocket() {

	/*
	 * AF_INET: IPv4
	 * SOCK_STREAM: Protocollo affidabile
	 * 0: Protocollo di default (TCP)
	 */
	int new_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (new_socket == -1) {
		perror("[Client] Errore durante la creazione del socket");
		return -1;
	}
	return new_socket;
}

int connect_to_server(int c_socket, char *s_ip, int s_port) {
	
	// Server socket creation
	struct sockaddr_in server_addr;			 // Creo una socket server
	memset(&server_addr, 0, sizeof(server_addr)); 	 // Libero memoria
	server_addr.sin_family = AF_INET; 		 // Protocollo IPv4
	server_addr.sin_addr.s_addr = inet_addr(s_ip);	 // IP del server
	server_addr.sin_port = htons(s_port);		 // Porta del server
	
	
	// Connection
	int conn_res = connect(c_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (conn_res == -1) {
		perror("[Client] Errore durante la connessione con il server\n");
		close(c_socket);
		return -1;
	}
	
	return 0;
	

}

int send_message(int c_socket, char *message, int mess_len, int flag) {
	int res = send(c_socket, message, mess_len, flag);
	
	return res;
}


int recv_message(int c_socket, char *buffer, int buff_len, int flag) {
	memset(buffer, 0, buff_len);
	int message = recv(c_socket, buffer, buff_len, flag);
	
	return message;
}
	
