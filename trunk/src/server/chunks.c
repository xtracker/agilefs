/**
 * src/server/chunks.c
 * 
 * Author: Jarvis
 * 
 * Date:2012-04-06
 * 
 * free chunk management implements
 *
 */

#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "chunks.h"


/**
 * build free chunk link from file
 *
 */
int init_free_chunk(struct free_chunk_list *pfcl, const char *path)
{
	int fd, total = 0, i = 0;

	memset(pfcl, 0, sizeof(struct free_chunk_list));
	pfcl->current = -1;

	if (access(path, F_OK))		//whether the file exsits
		return 0;
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		perror("init free chunk error : ");	//error ocurred when open the file
		return -1;
	}
	read(fd, pfcl, sizeof(int) * 2);
	total = pfcl->total_cnt;
	for (; total; ++i, total -= PER_LIST_SIZE) {
		pfcl->base[i] = (unsigned *)malloc(sizeof(unsigned) * PER_LIST_SIZE);
		read(fd, pfcl->base[i], sizeof(unsigned) * PER_LIST_SIZE);
	}
	close(fd);
	return 0;
}

/**
 * get the first free chunk's position
 *
 * return : -1 if there is no free chunk or the offset
 *			indentify the first free chunk
 */
int get_first_free_chunk(struct free_chunk_list *pfcl)
{
	int current = pfcl->current;
	if (-1 == current)
		return -1;
	else {
		--pfcl->current;
		return free_list(pfcl, current);
		//return pfcl->base[current >> 11][current & (PER_LIST_SIZE - 1)];
	}
}

/**
 * add one chunk to the list
 *
 * return 0
 */
int add_free_chunk(struct free_chunk_list *pfcl, int offset)
{
	int total = pfcl->total_cnt;
	int current = ++pfcl->current;
	if (current >= total) {
		unsigned *p = NULL;
		pfcl->total_cnt += PER_LIST_SIZE;
		p = (unsigned *)malloc(sizeof(unsigned) * PER_LIST_SIZE);
		if (!p)
			return -1;
		else
			pfcl->base[current >> PER_LIST_OFFSET] = p;
	}
	free_list(pfcl, current) = offset;
	//pfcl->base[current >> 11][current & (PER_LIST_SIZE - 1)] = offset;
	return 0;
}

/**
 * flush the free chunk list to file before server shuts down
 *
 */
int flush_free_chunk(struct free_chunk_list *pfcl, const char *path)
{
	int fd, total = pfcl->current, i = 0;
	fd = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
	if (fd == -1) {
		perror("flush error! : ");
		return -1;
	}

	total &= ~(PER_LIST_SIZE - 1);
	total += PER_LIST_SIZE;
	pfcl->total_cnt = total;
	write(fd, pfcl, sizeof(int) * 2);
	for (; total; ++i, total -= PER_LIST_SIZE) {
		write(fd, pfcl->base[i], sizeof(unsigned) * PER_LIST_SIZE);
		free(pfcl->base[i]);
	}
	close(fd);

	return 0;
}

//just used in debug mode
void printinfo(struct free_chunk_list *pfcl)
{
	printf("total_cnt = %d\ncurrent = %d\n", pfcl->total_cnt, pfcl->current);
}

