/**
 * src/server/chunks.h
 * 
 * Author: Jarvis
 * 
 * Date:2012-04-06
 * 
 * chunk server implements, free chunks operations
 *
 */
#ifndef _CHUNKS_H_
#define _CHUNKS_H_

#define FREE_CHUNK_MAX 1024
#define PER_LIST_SIZE 1024
#define PER_LIST_OFFSET 10

struct free_chunk_list { //free chunks table
	int total_cnt;
	int current;
	unsigned *base[FREE_CHUNK_MAX]; //up to 16TB per file
};

//free chunk list operations
#define free_list(fclp, current) \
	(fclp)->base[(current) >> PER_LIST_OFFSET][(current) & PER_LIST_SIZE]

void printinfo(struct free_chunk_list *fclp);

int init_free_chunk(struct free_chunk_list *fclp, const char *path);

int flush_free_chunk(struct free_chunk_list *fclp, const char *path);

int add_free_chunk(struct free_chunk_list *fclp, int offset);

int get_first_free_chunk(struct free_chunk_list *fclp);

struct chunk_file_info {
	int total;	
	int *fds;
	long *cur_size; // current size of a file (up to 4GB)
	struct free_chunk_list *fcls;
};

int init_chunk_file(struct chunk_file_info *base, const char *path);
int release_chunk_file(struct chunk_file_info *base, const char *path);


#endif //#ifdef _CHUNKS_H_
