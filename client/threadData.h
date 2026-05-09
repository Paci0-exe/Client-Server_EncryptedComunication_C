#ifndef THREADDATA_H
#define THREADDATA_H
#include <signal.h>
#include <stdbool.h>

typedef struct {

	unsigned char *input;
	unsigned char *key;
	unsigned char *output;

} ThreadData;


ThreadData* newThreadData(unsigned char*, unsigned char*);
ThreadData **create_td_list(unsigned char*, int, unsigned char*, bool, int*);
void *crypt_string_p(void*);
unsigned char *handlePThreads(unsigned char*, int, unsigned char*, int, bool, int*);
unsigned char *compose_message(ThreadData**, int, int*);
sigset_t *block_signals();
void unblock_signals(sigset_t *);

#endif
