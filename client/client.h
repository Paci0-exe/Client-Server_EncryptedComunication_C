#ifndef CLIENT_H
#define CLIENT_H

#include "threadData.h"

int createSocket();
int connect_to_server(int, char*, int);
int send_message(int, char*, int, int);
int recv_message(int, char*, int, int);

#endif
