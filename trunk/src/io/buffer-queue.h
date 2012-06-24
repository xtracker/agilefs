/**
 *
 *
 *
 */

#ifndef __BUFFER_QUEUE__
#define __BUFFER_QUEUE__ 1

#include "agilefs-def.h"

#define BUFF_QUEUE_SIZE 8

typedef struct chunk_data {	
	char data[FSP_SIZE];
} chunk_data_t;

typedef struct hash_key {
	char key[HASH_SIZE];
} hash_key_t;

struct buffer_queue {
	sem_t empty, full;
	pthread_mutex_t mutex;
	int size;
	int head, tail;
	struct hash_key *hq;
	struct chunk_data *cd;
};

#endif	//ops-queue.h
