/*
 * fuse_io_util.h
 *
 *  Created on: Jun 11, 2011
 *      Author: Zhao Guoyu <jarvis.xera@gmail.com>
 */

#ifndef FUSE_IO_UTIL_H_
#define FUSE_IO_UTIL_H_

inline long int get_min(long int a, long int b);

void md5_to_path(char*path);

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);

int write_block_file(const char*path, const unsigned char *buffer, size_t size);

int ulink_block_file(const char*path);

// useful macros for the lock file operations

#define unlock_reg(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_UNLCK, (offset), (whence), (len))

#define read_lock(fd, offset, whence, len) \
	lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))

#define write_lock(fd, offset, whence, len) \
	    lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))

#define block_lock(fd) \
	lock_reg((fd), F_SETLKW, F_WRLCK, 0, SEEK_SET, 0)

#define block_unlock(fd) \
	lock_reg((fd), F_SETLKW, F_UNLCK, 0, SEEK_SET, 0)


#endif /* FUSE_IO_UTIL_H_ */
