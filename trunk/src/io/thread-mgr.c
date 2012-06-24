/**
 *
 *
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#include "agilefs-def.h"
#include "buffer-queue.h"
#include "thread-mgr.h"

static struct buffer_queue bq = {
	.mutex	= PTHREAD_MUTEX_INITIALIZER,
	.size	= BUFF_QUEUE_SIZE,
	.head	= 0,
	.tail	= 0,
	.hq		= NULL,
	.cd		= NULL
};

static int produce_thread_running = 0;
static int consume_thread_running = 0;

void *producer_thread_fun(void *arg);
void *consumer_threads_fun(void *arg);

static thread_fun_type default_producer_fun = producer_thread_fun;
static thread_fun_type default_consumer_fun = consumer_threads_fun;

/**
 * initialize a buffer queue
 */
int init_buffer_queue(struct buffer_queue *bq)
{
	int ret = 0;
	ret = sem_init(&bq->empty, 0, BUFF_QUEUE_SIZE);
	if (ret)
		goto err;
	ret = sem_init(&bq->full, 0, 0);
	if (ret)
		goto full_init_err;
	bq->hq = (hash_key_t *)malloc(BUFF_QUEUE_SIZE * sizeof(hash_key_t));
	bq->cd = (chunk_data_t *)malloc(BUFF_QUEUE_SIZE * sizeof(chunk_data_t));
	if (bq->hq && bq->cd)
		return 0;
malloc_err:
	sem_destroy(&bq->full);
full_init_err:
	sem_destroy(&bq->empty);
err:
	return ret;
}

/**
 * start the producer thread, if fn is NULL, the defualt thread function will
 * be used, otherwise the function pointed by fn will be used instead
 *
 */
int producer_thread_start(pthread_t *tid, void *(*fn)(void *), void *arg)
{
	int ret = 0;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	if (ret)
		goto err;
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret)
		goto set_attr_err;

	produce_thread_running = 1;
	if (fn == NULL)
		ret = pthread_create(tid, &attr, default_producer_fun, arg);
	else
		ret = pthread_create(tid, &attr, fn, arg);
	if (!ret) {
		return 0;
	} else
		produce_thread_running = 0;

set_attr_err:
	pthread_attr_destroy(&attr);
err:
	return ret;
}

int consumer_thread_start(pthread_t *tid, void *(*fn)(void *), void *arg)
{
	int ret = 0;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	if (ret)
		goto err;
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret)
		goto set_attr_err;

	consume_thread_running = 1;
	
	ret = pthread_create(tid,
			&attr, 
			fn ? fn : default_consumer_fun,
			arg);
	
	if (!ret) {
		return 0;
	} else
		consume_thread_running = 0;

set_attr_err:
	pthread_attr_destroy(&attr);
err:
	return ret;
}

void *producer_thread_fun(void *arg)
{
	struct buffer_queue *bq = ((struct thread_io_info *)arg)->bq;
	struct thread_io_operations *tio_ops = ((struct thread_io_info *)arg)->tio_ops;
	
	while (produce_thread_running) {
		produce_one_item();
		
		sem_wait(&bq->empty);
		put_item_in();
		//pthread_mutex_lock(&bq->mutex);
		bq->tail = (bq->tail + 1) & (BUFF_QUEUE_SIZE - 1);
		//pthread_mutex_unlock(&bq->mutex);
		sem_post(&bq->full);
	}
	return ((void *)0);
}

void *consumer_threads_fun(void *arg)
{
	struct buffer_queue *bq = ((struct thread_io_info *)arg)->bq;
	struct thread_io_operations *tio_ops = ((struct thread_io_info *)arg)->tio_ops;
	char chunk_data[FSP_SIZE];
	char hash_key[HASH_SIZE];

	while (consume_thread_running) {
		sem_wait(&bq->full);
		pthread_mutex_lock(&bq->mutex);

		memcpy(hash_key, bq->hq[bq->head].key, HASH_SIZE);
		memcpy(chunk_data, bq->cd[bq->head].data, FSP_SIZE);
		
		bq->head = (bq->head + 1) & (BUFF_QUEUE_SIZE - 1);
		pthread_mutex_unlock(&bq->mutex);
		sem_post(&bq->empty);
		tio_ops->put_new_chunk();
	}
	return (void *)0;
}

