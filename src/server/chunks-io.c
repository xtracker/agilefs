/**
 *
 *	filename :	chunks-io.c
 *	Author	:	Zhao Guoyu
 *	Date	:	2012-05-30
 *	Descript
 */

#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>

#include "agilefs-def.h"
#include "chunks.h"
#include "chunks-io.h"
#include "db_ops.h"

extern struct chunk_file_info cfi;

int get_proper_chunk_file(struct chunk_file_info *cfip)
{
	int ret = -1, i = 0;
	long __min = 0xffffffff, capcity, current;
	for (; i < cfip->total; ++i) {
		capcity = cfip->cur_size[i];
		current = (long) cfip->fcls[i].current;
		capcity -= (current == -1 ? 0 : (current << FSP_OFFSET));
		if (capcity < MAX_CHUNK_FILE_SIZE && __min > capcity)
			__min = capcity, ret = i;
	}
	return ret;
}

/**
 *
 *
 *
 * return	: the size of data wrtien to file
 *				otherwise -1
 */
int block_write(void *key, const char *buf,
		size_t size,
		struct chunk_file_info *cfip)
{
	int ret = 0, fd_index;
	long offset = 0;
	struct free_chunk_list *fclp = NULL;
	
	fd_index = get_proper_chunk_file(cfip);
	assert(fd_index >= 0);
	
	fclp = &cfip->fcls[fd_index];
	ret = get_first_free_chunk(fclp);

	if (ret == -1) {
		offset = cfip->cur_size[fd_index];
		cfip->cur_size[fd_index] += FSP_SIZE;
	}
	else {
		offset = (((off_t)ret) << FSP_OFFSET);
	}

//	printf("write to %d offset is %d\n", fd_index, offset);
	ret = pwrite(cfip->fds[fd_index], buf, size, offset);
	
	if (ret == FSP_SIZE) {
		int x = write_hash_db(key, offset, fd_index);
		if (x) {
			ret = -1;
			db_err_log(x, "hash error");
		}
	}
	return ret;
}

/**
 *
 *
 *	return	:	size of data read, otherwise -1
 */
int block_read(void *key, char *buf, size_t size,
		struct chunk_file_info *cfip)
{
	int ret = 0, fd = -1;
	struct block_data bd = { 0 };
	ret = db_get(key, &bd);
	
	if (ret) {
		db_err_log(ret, "retrive key error\n");
		return -1;
	}
	printf("read share_num is %d\n", bd.ref_count);
	fd = cfip->fds[bd.fd_index];
	ret = pread(fd, buf, size, bd.offset);
	return ret;
}

/**
 *
 *
 *
 *
 */
int put_new_chunk(void *key, const char *buf, size_t size,
		struct chunk_file_info *cfip)
{
	int ret = 0;
	struct block_data bd = {0};
	ret = db_get(key, &bd);
	if (ret) {
		ret = block_write(key, buf, size, cfip);
		ret = (ret == FSP_SIZE ? 0 : -1);
	}
	else {
		++bd.ref_count;
		ret = db_put(key, &bd);
		if (ret) {
			db_err_log(ret, "update hash db error :");
			ret = -1;
		}
	}
	return ret;
}

/**
 *
 *
 *
 *
 */
int del_chunk(void *key, struct chunk_file_info *cfip)
{
	int ret = 0;
	struct block_data bd = {0};
	ret = db_get(key, &bd);
	
	if (ret) {
		
		if (bd.ref_count > 1) {
			--bd.ref_count;
			ret = db_put(key, &bd);
		}
		else {
			add_free_chunk(&cfip->fcls[bd.fd_index],
					(unsigned)(bd.offset >> FSP_OFFSET));
			
			ret = db_del(key);
		}
	}
	return ret;
}


int sync_chunk_data(struct chunk_file_info *cfip)
{
	int i = 0, ret = 0;
	for (i = 0; i < cfip->total; ++i) {
		ret = fsync(cfip->fds[i]);
		if (!ret) {
			printf("[WARNING]FSYNC ERROR: %s", strerror(errno));
		}
	}
	return 0;
}

