/**
 *
 *	filename :	chunk-io.c
 *	Author	:	Zhao Guoyu
 *	Date	:	2012-05-30
 *	Descript
 */

#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <fcntl.h>

#include "agilefs-def.h"
#include "chunks.h"
#include "chunks-io.h"

extern struct chunk_file_info cfi;

int get_proper_chunk_file(struct chunk_file_info *pcfi)
{
	int ret = -1, i = 0;
	long __min = 0xffffffffu;
	for (; i < pcfi->total; ++i)
	{
		long capcity = pcfi->cur_size[i];
		long current = (long) pcfi->fcls[i].current;
		capcity -= current == -1 ? 0 : (current << FSP_OFFSET);
		if (capcity < MAX_CHUNK_FILE_SIZE && __min > capcity)
			__min = capcity, ret = i;
	}
	return ret;
}

int block_write(const char *buf, size_t size, struct chunk_file_info *pcfi)
{
	int ret = 0, proper_index = -1;
	long offset = 0;
	struct free_chunk_list *pfcl = NULL;
	
	proper_index = get_proper_chunk_file(pcfi);
	if (proper_index == -1)
		return -1;
	pfcl = &pcfi->fcls[proper_index];
	ret = get_first_free_chunk(pfcl);
	if (ret == -1)
		offset = pcfi->cur_size[proper_index];
	else
		offset = ((off_t)ret) << FSP_OFFSET;
	ret = pwrite(pcfi->fds[proper_index], buf, size, offset);
	if (ret == FSP_SIZE)
		pcfi->cur_size[proper_index] += ret;
	return ret;
}
