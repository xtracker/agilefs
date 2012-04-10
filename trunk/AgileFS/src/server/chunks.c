#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

#include "chunks.h"



ssize_t write_chunk(const char *buf, off_t offset, size_t size)
{
	return pwrite(1, buf, size, offset);
}

