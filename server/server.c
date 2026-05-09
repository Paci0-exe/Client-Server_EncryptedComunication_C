#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "gestore.h"
#include "inputHandler.h"

int main(int argc, char *argv[]) /* argv[1]= porta
									argv[2]= p (parallelismo dei thread di decifratura,
								    argv[3]= s (prefisso file OutPut),
								    argv[4]= I (max connessioni client da gestire contemporaneamente)  */
{
  handler *h = newHandler(argc, argv);
  int p = atoi(h->parallelism);
  char* s = h -> file_name;
  int I = atoi(h -> connection);
  /*
  p=atoi(argv[2]);
  s=argv[3];
  I=atoi(argv[4]);*/
  
  int sfd, client_sfd;
  int tcp_port = atoi(h -> port); 
  socklen_t client_size;
  struct sockaddr_in server_address, client_addr;
  sigset_t set;

  //tcp_port=atoi(argv[1]);
  sfd = socket(AF_INET, SOCK_STREAM, 0);/* Bidirectional flow, IPv4 */
  if (sfd == -1) {
    perror("[Server] errore in apertura socket");
    return 1;
  }
  
  memset((char *) &server_address, 0, sizeof(server_address));
  server_address.sin_family=AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port=htons(tcp_port);

  if (bind(sfd, (struct sockaddr *) &server_address,
  sizeof(server_address)) == -1 ) {
    perror("[Server] In binding");
    return 1;
  }
  
  printf("[Server] Server in ascolto sulla porta %d ...\n", tcp_port);
  listen(sfd,I);
  client_size = sizeof(client_addr);

  /* disabilito segnale SIGCHLD */
  sigemptyset(&set);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, NULL);
  //signal(SIGCHLD, SIG_IGN); /*per non creare zombie*/
  while (1) { /* accetto indefinitamente
  * connessioni dai client */
    printf("[Server] Connessione client...\n");
    client_sfd = accept(sfd, (struct sockaddr *) &client_addr, &client_size);
    if (client_sfd < 0){
      printf("[Server] Errore accettando connessione\n");
      continue;
    }
  
    if (fork()==0) {
      gestore(client_sfd, s, p);
      close(client_sfd);
      printf("[Server] Chiusura connessione\n");
      return 0;
    }
  }
  return 0;
}
