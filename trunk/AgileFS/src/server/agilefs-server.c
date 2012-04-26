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

int main(int agrc, char **argv)
{
	struct free_chunk_link fcl;
	printf("sizeof(unsigned int) is %d\n", sizeof(int));
	init_free_chunk(&fcl, "/home/jarvis/free.dat");
	printinfo(&fcl);
	add_free_chunk(&fcl, 0);
	printinfo(&fcl);
//	get_first_free_chunk(&fcl);
	flush_free_chunk(&fcl, "/home/jarvis/free.dat");
	return 0;
}
