/**
 *
 *
 *
 */
#ifndef __THREAD_MGR_H__
#define __THREAD_MGR_H__

#include "chunks-io.h"

typedef void *(*thread_fun_type)(void *);

struct thread_io_operations {
	int (*put_new_chunk)(void *key, char *buf, size_t size,
			struct chunk_file_info *pcfi);
	int (*get_one_chunk)();
};

struct thread_io_info {
	struct buffer_queue *bq;
	struct thread_io_operations *tio_ops;
};

int producer_thread_start(pthread_t *tid, void *(*fn)(void *), void *arg);
int consumer_thread_start(pthread_t *tid, void *(*fn)(void *), void *arg);

#endif
