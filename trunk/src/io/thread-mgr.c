/**
 * thread_mgr.c
 * Zhao Guoyu
 * Date: 2012
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
#include "chunks-io.h"
#include "thread-mgr.h"

#include "buffer-queue.h"
extern int over;

static struct buffer_queue bq_array[NUM_BUFFER_QUEUES];

/**
static struct buffer_queue bq = {
	.mutex	= PTHREAD_MUTEX_INITIALIZER,
	.head	= 0,
	.tail	= 0,
	.hk		= NULL,
	.cd		= NULL,
};
*/
static struct thread_io_operations tio_ops = {
	.put_new_chunk = put_new_chunk,
	.get_one_chunk = test_get_one_chunk,
};

struct thread_io_context tio_info = {
	.producer_threads_num = 1,
	.consumer_threads_num = NUM_SERVER_THREADS,
	.cfi = &cfi,
	.bqs = bq_array,
	/** bq = **/
	.tio_ops = &tio_ops,
};

int thread_io_start(void)
{
	int ret = 0, i,
		p_count = tio_info.producer_threads_num,
		c_count = tio_info.consumer_threads_num;
	
	pthread_t producer_id, consumer_id;
	
	for (i = 0; i < NUM_BUFFER_QUEUES; ++i)	{
		ret = init_buffer_queue(&tio_info.bqs[i]);
		if (ret)
			return ret;
	}
	while (p_count--) {
		ret = producer_thread_start(&producer_id, NULL, NULL);
		if (ret)
			return ret;
	}
	for (i = 0; i < c_count; ++i) {
		ret = consumer_thread_start(&consumer_id, NULL, (void *) (long)i);
		if (ret)
			return ret;
	}
	return ret;
}

static int producer_thread_running = 0;
static int consumer_thread_running = 0;


void *producer_thread_fun(void *arg);
void *consumer_thread_fun(void *arg);

static thread_fun_type default_producer_fun = producer_thread_fun;
static thread_fun_type default_consumer_fun = consumer_thread_fun;

/**
 * initialize a buffer queue
 */
int init_buffer_queue(struct buffer_queue *bq)
{
	int ret = 0;
	bq->head = 0, bq->tail = 0;
	if (ret)
		return ret;
	ret = pthread_mutex_init(&bq->mutex, NULL);
	ret = sem_init(&bq->empty, 0, BUFF_QUEUE_SIZE);
	if (ret)
		goto err;
	ret = sem_init(&bq->full, 0, 0);
	if (ret)
		goto full_init_err;
	bq->hk = (hash_key_t *) malloc(BUFF_QUEUE_SIZE * sizeof(hash_key_t));
	bq->cd = (chunk_data_t *) malloc(BUFF_QUEUE_SIZE * sizeof(chunk_data_t));
	if (bq->hk && bq->cd)
		return 0;
malloc_err:
	sem_destroy(&bq->full);
full_init_err:
	sem_destroy(&bq->empty);
err:
	pthread_mutex_destroy(&bq->mutex);
	return ret;
}

/**
 * start the producer thread, if fn is NULL, the defualt thread function will
 * be used, otherwise the function pointed by fn will be used instead
 *
 */
int producer_thread_start(pthread_t *tid, void *(*fn)(void *),
		struct thread_io_context *tio_info)
{
	int ret = 0;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	if (ret)
		goto err;
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret)
		goto set_attr_err;

	producer_thread_running = 1;
	if (fn == NULL)
		ret = pthread_create(tid, &attr, default_producer_fun, tio_info);
	else
		ret = pthread_create(tid, &attr, fn, tio_info);
	if (!ret) {
		return 0;
	} else
		producer_thread_running = 0;

set_attr_err:
	pthread_attr_destroy(&attr);
err:
	return ret;
}

int consumer_thread_start(pthread_t *tid, void *(*fn)(void *),
		struct thread_io_context *tio_info)
{
	int ret = 0;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	if (ret)
		goto err;
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret)
		goto set_attr_err;

	fn = fn ? fn : default_consumer_fun;

	consumer_thread_running = 1;
	
	ret = pthread_create(tid, &attr, fn, tio_info);
	
	if (!ret) {
		return 0;
	} else
		consumer_thread_running = 0;

set_attr_err:
	pthread_attr_destroy(&attr);
err:
	return ret;
}

int thread_io_finalize(struct thread_io_context *tio_info)
{
	/**
	producer_thread_running = 0;
	consumer_thread_running = 0;
	sem_destroy(&tio_info->bq->full);
	sem_destroy(&tio_info->bq->empty);
	pthread_mutex_destroy(&tio_info->bq->mutex);
	**/
	return 0;
}

void *producer_thread_fun(void *arg)
{
	int ret = 0, hash_index;
	char chunk_data[FSP_SIZE], hash_key[HASH_SIZE];
	struct buffer_queue *bq = NULL;
	struct thread_io_operations *tio_ops = tio_info.tio_ops;

	while (producer_thread_running) {
		ret = tio_ops->get_one_chunk(chunk_data, hash_key, FSP_SIZE);
		if (ret <= 0)
			break;
		
		hash_index = *((int *)hash_key);
		bq = &tio_info.bqs[hash_index];
		sem_wait(&bq->empty);
		//pthread_mutex_lock(&bq->mutex);
		memcpy(bq->cd[bq->tail].data, chunk_data, FSP_SIZE);
		memcpy(bq->hk[bq->tail].key, hash_key, HASH_SIZE);
		bq->tail = (bq->tail + 1) & (BUFF_QUEUE_SIZE - 1);
		//pthread_mutex_unlock(&bq->mutex);
		sem_post(&bq->full);
	}
	over = 1;
	return ((void *)0);
}

void *consumer_thread_fun(void *arg)
{	
	char chunk_data[FSP_SIZE];
	char hash_key[HASH_SIZE];
	int ret = 0;
	int thread_index = (int)arg;

	//struct thread_io_context *tio_info = 
	struct buffer_queue *bq = &tio_info.bqs[thread_index];
	struct thread_io_operations *tio_ops = tio_info.tio_ops;

	while (consumer_thread_running) {
		sem_wait(&bq->full);

		pthread_mutex_lock(&bq->mutex);
		memcpy(hash_key, bq->hk[bq->head].key, HASH_SIZE);
		memcpy(chunk_data, bq->cd[bq->head].data, FSP_SIZE);
		bq->head = (bq->head + 1) & (BUFF_QUEUE_SIZE - 1);
		pthread_mutex_unlock(&bq->mutex);

		sem_post(&bq->empty);
		ret	= tio_ops->put_new_chunk(hash_key, chunk_data, FSP_SIZE, tio_info.cfi);
	}
	return (void *)0;
}

