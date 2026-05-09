#ifndef MESSAGE_H
#define MESSAGE_H


typedef struct {
	unsigned char *mess;
	int length;
	unsigned char *key;
	//int len_tot;
} message;


unsigned char *createMessage(unsigned char*, int, unsigned char*, int, int*);
message *readMessage(unsigned char*);


#endif
