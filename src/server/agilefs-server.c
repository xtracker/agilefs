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
#include <sys/stat.h>

#include "agilefs-def.h"

#include "chunks.h"
#include "chunks-io.h"

//forward declaration of global free_chunk_link struct

extern int meta_server_init(const char *db_path);
extern int meta_server_close();

struct chunk_file_info cfi = { 0 };

char buf[4096];
int main(int agrc, char **argv)
{
	int ret = 0;
	printf("current process ID is %d\n", (int)getpid());
	umask(0);
	meta_server_init("/home/jarvis/agilefs-hash.db");
	init_chunk_file(&cfi, "global.xml");
	strcpy(buf, "this is a test of agilefs!!\n");
	ret = block_write("0", buf, 4096, &cfi);
	memset(buf, 0 , sizeof(buf));
	ret = block_read("0", buf, 4096, &cfi);
	printf("%d\n content is %s\n", ret, buf);
	release_chunk_file(&cfi, "global.xml");
	meta_server_close();
	getchar();
	return 0;
}
