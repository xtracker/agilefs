/**
 *
 *
 *
 */

#ifndef __BUFFER_QUEUE__
#define __BUFFER_QUEUE__

#include "agilefs-def.h"
#include <pthread.h>
#include <semaphore.h>

#ifdef __cpluscplus
extern "C" {
#endif

#define BUFF_QUEUE_SIZE 8

typedef struct chunk_data {	
	char data[FSP_SIZE];
} chunk_data_t, chunk_data_p;

typedef struct hash_key {
	char key[HASH_SIZE];
} hash_key_t, hash_key_p;

struct buffer_queue {
	sem_t empty, full;
	pthread_mutex_t mutex;
	int head, tail;
	struct hash_key *hk;
	struct chunk_data *cd;
};

typedef struct buffer_queue buffer_queue_t;
typedef struct buffer_queue *buffer_queue_p;

#ifdef __cpluscplus
}
#endif /** __cpluscplus **/
#endif	//ops-queue.h
