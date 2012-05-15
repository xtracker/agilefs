/**
 * src/server/agilefs-server.c
 * 
 * Author: Jarvis
 * 
 * Date:2012-04-06
 * 
 * server implements
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include "agilefs-def.h"
#include "chunks.h"

//forward declaration of global free_chunk_link struct

struct chunk_file_info cfi = {
	.fds = NULL,
	.fcls = NULL,
	.total = 0
};

int main(int agrc, char **argv)
{
	printf("current process ID is %d\n", (int)getpid());
//	umask(0);
	init_chunk_file(&cfi, "global.xml");
	getchar();
	release_chunk_file(&cfi, "global.xml");
	getchar();
	return 0;
}
