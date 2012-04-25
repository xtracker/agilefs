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

#define FREE_CHUNK_MAX 512
#define PER_LINK_SIZE 2048
#define PER_LINK_OFFSET 11

struct free_chunk_link { //free chunks table
	int total_cnt;
	int current;
	size_t *base[FREE_CHUNK_MAX];
};

#define free_link(pfcl, current) \
	(pfcl)->base[(current) >> 11][(current) & PER_LINK_SIZE]

ssize_t write_chunk(const char *buf, off_t offset, size_t size);

//free chunk link operations
int init_free_chunk(struct free_chunk_link *pfcl, const char *path);
int flush_free_chunk(struct free_chunk_link *pfcl, const char *path);
int add_free_chunk(struct free_chunk_link *pfcl, int offset);
int get_first_free_chunk(struct free_chunk_link *pfcl);

#endif
