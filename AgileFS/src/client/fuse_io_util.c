/*
 * fuse_io_util.c
 *
 *  Created on: June 11, 2011
 *      Author: Zhao Guoyu <jarvis.xera@gmail.com>
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "fuse_io_util.h"
#include <sys/xattr.h>
#include <errno.h>

/**
 * @func compare two long int values
 * @param a
 * @param b
 * @return min of the two values
 */
inline long int get_min(long int a, long int b)
{
	return a < b ? a : b;
}

/**
 * @func convert md5 value to ascii code to reperent its path
 * @param
 * @return void
 */
#ifdef __USE_SHA1

void md5_to_path(char* path)
{
	int i = 3, j = 21;
	sprintf(path, "%02x", *((unsigned char*) (path + 20)));
	*(path + 2) = '/';
	for (; i < 41 && j < 40; i += 2, ++j)
		sprintf(path + i, "%02x", *((unsigned char*) (path + j)));
	*(path + 41) = '\0';
}

#else

void md5_to_path(char* path)
{
	int i = 3, j = 21;
	sprintf(path, "%02x", *((unsigned char*) (path + 20)));
	*(path + 2) = '/';
	for (; i < 33 && j < 36; i += 2, ++j)
		sprintf(path + i, "%02x", *((unsigned char*) (path + j)));
	*(path + 33) = '\0';

}
#endif


/**
 * @func set file region lock using fcntl, it is just a wrapper of the
 *			fcntl function
 * @param descriptor of the file to be locked
 * @param
 * @param
 * @param
 * @param
 * @param
 * @return fcntl
 */
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock lock = {
		.l_type		= type,
		.l_start	= offset,
		.l_whence	= whence,
		.l_len		= len
	};
	return fcntl(fd, cmd, &lock);
}

/**
 * @func write one block file, first we exames whether the file to be writed is
 *			already there by the access system call with param F_OK, if the file
 *			exists, we just increase its share_cnt xattr, otherwise we create the
 *			file first, and then make its share_cnt value to 1
 * @param path of the block file
 * @param contains the data to be writen to the file
 * @param size of the buffer
 * @return 0 if successful or errno if not
 */
int write_block_file(const char* path, const unsigned char *buffer, size_t size)
{
	int fd;
	int err = 0;
	int share_num = 1;
	int res = access(path, F_OK);
	if (res != 0)
	{

		//fd = creat(path, 0766);
		fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0766);
		if (fd == -1)
		{
			perror("creat new file error");
			return -errno;
		}

		err = write(fd, buffer, size);
		if (err < size)
			return -errno;
		err = fsetxattr(fd, "user.share_cnt", &share_num, sizeof(int),
				XATTR_CREATE);
		close(fd);

	} else
	{

		getxattr(path, "user.share_cnt", &share_num, sizeof(int));
		++share_num;
		setxattr(path, "user.share_cnt", &share_num, sizeof(int),
				XATTR_REPLACE);
	}
	return 0;
}

/**
 * @func delete a block file.
 *			When deleting a file, first, we decrease its share_cnt xattr, then 
 *			we delete it if the share_cnt reaches 0 or just descrese its share_cnt
 *			xattr by 1
 * @param path of the block file
 * @reurn 0 if successful or errno if not
 */
int ulink_block_file(const char* path)
{
	int share_num = 0;
	int ret = getxattr(path, "user.share_cnt", &share_num, sizeof(int));

	if (--share_num == 0)
		ret = unlink(path);
	else
		ret = setxattr(path, "user.share_cnt", &share_num, sizeof(int), XATTR_REPLACE);
	return ret;
	//return 0;
}
