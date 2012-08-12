/**
 *
 *
 *
 */
#ifndef __THREAD_MGR_H__
#define __THREAD_MGR_H__

#include "buffer-queue.h"

extern int test_get_one_chunk(void *key, char *buf, size_t size);

typedef void *(*thread_fun_type)(void *);

struct thread_io_operations {
	int (*put_new_chunk)(void *key, const char *buf, size_t size,
			struct chunk_file_info *pcfi);
	int (*get_one_chunk)(void *key, char *buf, size_t size);
};

struct thread_io_context {
	int producer_threads_num, consumer_threads_num;
	struct chunk_file_info *cfi;
	struct buffer_queue *bqs;
	struct thread_io_operations *tio_ops;
};


int producer_thread_start(pthread_t *tid, void *(*fn)(void *),
		struct thread_io_context *tio_info);
int consumer_thread_start(pthread_t *tid, void *(*fn)(void *),
		struct thread_io_context *tio_info);

int thread_io_start();

#endif
