/**
 * global configuratins of agilefs system
 *
 *
 */
#ifndef __AGILEFS_DEF_H__
#define __AGILEFS_DEF_H__


/**
 * client side configurations
 */
#define FSP_SIZE 4096
#define FSP_OFFSET 12

#ifdef __USE_SHA1__
#define HASH_SIZE 20
#else
#define HASH_SIZE 16
#endif

/**
 * server configurations
 */
#define MAX_CHUNK_FILE_SIZE (1L << 32)
#define NUM_SERVER_THREADS 4
#define NUM_BUFFER_QUEUES NUM_SERVER_THREADS
#define NUM_SERVER_DBS NUM_SERVER_THREADS

/**
 * global definitions
 */

#if !defined hash_to_index
#define hash_to_index(hash_key) \
	*((int *)hash_key)
#endif

#endif


#define NUM_SERVERS 2
