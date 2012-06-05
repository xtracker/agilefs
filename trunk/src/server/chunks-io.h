/**
 *
 *
 *
 */

#ifndef __CHUNKS_IO_H__
#define	__CHUNKS_IO_H__

extern struct chunk_file_info cfi;	//declaration

extern int write_hash_db(void *key, long offset, int f_index);

//int get_proper_chunk_file(struct chunk_file_info *pcfi);

int block_write(void *key, const char *buf, size_t size, struct chunk_file_info *pcfi);

int block_read(void *key, char *buf, size_t size, struct chunk_file_info *pcfi);

int block_write_exclusive();

int put_new_chunk(void *key, const char *buf, size_t size, struct chunk_file_info *pcfi);

#endif
