/**
 *
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

/**
 *
 *
 */
int direct_write_at(int fd, char *buf, off_t offset, size_t size)
{
	int ret = 0;
	int ret_size = 0;
	do {
		ret = pwirte(fd,
				buf + ret_size,
				offset + ret_size,
				size - ret_size);
		ret_size += ret > 0 ? ret : 0;

	} while ((ret == -1 && errno == EINTR) || ret_size < size && ret > 0);
	
	if (ret == -1)
		return ret;
	else
		return ret_size;
}

/**
 *
 *
 */
int direct_read_at(int fd, char *buf, off_t offset, size_t size)
{
	int ret = 0, ret_size = 0;
	do {
		ret = pread(fd,
				buf + ret_size,
				offset + ret_size,
				size - ret_size);
		ret_size += ret > 0 ? ret : 0;
	} while ((ret == -1 && errno == EINTR) || ret_size < size && ret > 0);
	if (ret == -1)
		return ret;
	else
		return ret_size;
}
