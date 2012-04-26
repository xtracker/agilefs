/**
 * src/server/chunks.c
 * 
 * Author: Jarvis
 * 
 * Date:2012-04-06
 * 
 * chunk server implements
 *
 */

#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "chunks.h"


/**
 * build free chunk link from file
 *
 */
int init_free_chunk(struct free_chunk_link *pfcl, const char *path)
{
	int fd, total = 0, i = 0;

	memset(pfcl, 0, sizeof(struct free_chunk_link));
	pfcl->current = -1;

	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		perror("init error!\n");
		return -1;
	}
	read(fd, pfcl, sizeof(int) * 2);
	total = pfcl->total_cnt;
	for (; total; ++i, total -= PER_LINK_SIZE) {
		pfcl->base[i] = (unsigned *)malloc(sizeof(unsigned) * PER_LINK_SIZE);
		read(fd, pfcl->base[i], sizeof(unsigned) * PER_LINK_SIZE);
	}
	close(fd);
	return 0;
}

/**
 * get the first free chunk's position
 *
 *
 */
int get_first_free_chunk(struct free_chunk_link *pfcl)
{
	int current = pfcl->current;
	if (-1 == current)
		return -1;
	else
	{
		--pfcl->current;
		return free_link(pfcl, current);
		//return pfcl->base[current >> 11][current & (PER_LINK_SIZE - 1)];
	}
}

/**
 * add one chunk to the list
 *
 */
int add_free_chunk(struct free_chunk_link *pfcl, int offset)
{
	int total = pfcl->total_cnt;
	int current = ++pfcl->current;
	if (current >= total) {
		pfcl->total_cnt += PER_LINK_SIZE;
		pfcl->base[current >> 11] = (unsigned *)malloc(sizeof(unsigned) * PER_LINK_SIZE);
	}
	free_link(pfcl, current) = offset;
	//pfcl->base[current >> 11][current & (PER_LINK_SIZE - 1)] = offset;
	return 0;
}


int flush_free_chunk(struct free_chunk_link *pfcl, const char *path)
{
	int fd, total = pfcl->current, i = 0;
	fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd == -1)
	{
		perror("flush error!\n");
		return -1;
	}

	total &= ~(PER_LINK_SIZE - 1);
	total += PER_LINK_SIZE;
	pfcl->total_cnt = total;
	write(fd, pfcl, sizeof(int) * 2);
	for (; total; ++i, total -= PER_LINK_SIZE)
	{
		write(fd, pfcl->base[i], sizeof(unsigned) * PER_LINK_SIZE);
		free(pfcl->base[i]);
	}
	close(fd);

	return 0;
}

void printinfo(struct free_chunk_link *pfcl)
{
	printf("total_cnt = %d\ncurrent = %d\n", pfcl->total_cnt, pfcl->current);
}

