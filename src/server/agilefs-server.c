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
#include "md5.h"
#include "fuse_io_util.h"

#include "thread-mgr.h"
//forward declaration of global free_chunk_link struct

extern int meta_server_init(const char *db_path);
extern int meta_server_close();

struct chunk_file_info cfi = { 0 };

int fd = -1;

int over = 0;
char buf[4096];
char hash[20] = {0};

int test_get_one_chunk(void *key, char *buf, size_t size)
{
	int ret = read(fd, buf, FSP_SIZE);
	if (ret > 0) {
		md5(buf, HASH_SIZE, key);
	}
	return ret;
}

int main(int agrc, char **argv)
{
	int ret = 0;
	printf("current process ID is %d\n", (int)getpid());
	umask(0);
	ret = meta_server_init("/home/jarvis/agilefs-hash.db");
	ret = init_chunk_file(&cfi, "/home/jarvis/global.xml");
	system("date");
	fd = open(argv[1], O_RDONLY);	
	ret = thread_io_start();
	while (!over);
	system("date");
	close(fd);
	release_chunk_file(&cfi, "/home/jarvis/global.xml");
	meta_server_close();
	return 0;
}
