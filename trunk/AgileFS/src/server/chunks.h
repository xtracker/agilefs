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

#define FREE_CHUNK_MAX 1024
#define PER_LINK_SIZE 1024
#define PER_LINK_OFFSET 10

struct free_chunk_link { //free chunks table
	int total_cnt;
	int current;
	unsigned *base[FREE_CHUNK_MAX]; //up to 16TB per file
};

//free chunk link operations
#define free_link(pfcl, current) \
	(pfcl)->base[(current) >> PER_LINK_OFFSET][(current) & PER_LINK_SIZE]

void printinfo(struct free_chunk_link *pfcl);
int init_free_chunk(struct free_chunk_link *pfcl, const char *path);
int flush_free_chunk(struct free_chunk_link *pfcl, const char *path);
int add_free_chunk(struct free_chunk_link *pfcl, int offset);
int get_first_free_chunk(struct free_chunk_link *pfcl);

#endif //#ifdef _CHUNKS_H_
