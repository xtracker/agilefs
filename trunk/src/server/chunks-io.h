/**
 *
 *
 *
 */

#ifndef __CHUNKS_IO_H__
#define	__CHUNKS_IO_H__

extern struct chunk_file_info cfi;	//declaration

int get_proper_chunk_file(struct chunk_file_info *pcfi);
int block_write(const char *buf, size_t size, struct chunk_file_info *pcfi);
int block_write_exclusive();


#endif

