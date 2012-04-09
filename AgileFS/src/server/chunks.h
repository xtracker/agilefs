/**
 * src/server/chunks.h
 * 
 * Author: Jarvis
 * 
 * Date:2012-04-06
 * 
 * chunk server implements
 *
 */
#ifndef _CHUNKS_H_
#define _CHUNKS_H_

struct free_chunk_link{
	int total_size;
	int current;
	size_t *base;
};

struct free_chunk_operations{

	int (*get_current)();
};
ssize_t write_chunk(const char *buf, off_t offset, size_t size);

size_t get_first_free_chunk(const struct free_chunk_link *pfcl);

void free_chunk(size_t offset);

#endif
