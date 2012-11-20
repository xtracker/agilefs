/**
 *
 * filename:	agilefs-fuse.c
 * Copyright (C) 2011 Zhao Guoyu <jarvis.xera@gmail.com>
 *
 */
#define FUSE_USE_VERSION 28

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#include "md5.h"
#include "fuse_type.h"
#include "fuse_cache.h"
#include "fuse_io_util.h"
#include "chunks.h"
#include "chunks-io.h"

#define BLOCK_SIZE 4096
static const off_t g_block_size = BLOCK_SIZE; //64K=0x10000
static const off_t init_offset = 16;

#ifdef __USE_SHA1
#define hash_size  20
#else
#define hash_size  16
#endif

/**
 *
 * file lock macros
 */
#define read_lock_metafile(fd, offset, len) \
	read_lock(fd, (offset / g_block_size * hash_size + init_offset), \
			 SEEK_SET, (len + g_block_size) / g_block_size * hash_size );

#define read_unlock_metafile(fd, offset, len) \
	unlock_reg(fd, (offset / g_block_size * hash_size + init_offset), \
			SEEK_SET, (len + g_block_size) / g_block_size * hash_size );

#define write_lock_metafile(fd, offset, len)	\
	write_lock(fd, (offset / g_block_size * hash_size + init_offset), \
			SEEK_SET, (len + g_block_size) / g_block_size * hash_size );

#define write_unlock_metafile(fd, offset, len) \
	unlock_reg(fd, (offset / g_block_size * hash_size + init_offset), \
			SEEK_SET, (len + g_block_size) / g_block_size * hash_size);



struct chunk_file_info cfi = { 0 };
extern int meta_server_close();
extern int meta_server_init(const char *db_path);


/**
 * @func	equal to the stat/lstaf system call, the difference between the
 *			fuse_getattr and the stat is that we get the size of the file by
 *			reading the first 8 bytes of the file
 * @param	path of the file
 * @param	the stat struct to be filled
 * @return	0 if successful or other if not
 */
static int fuse_getattr(const char *path, struct stat *stbuf)
{
	int res;

	res = lstat(path, stbuf);
	if (res == -1)
		return -errno;

	if (stbuf->st_mode & S_IFREG)
	{
		res = open(path, O_RDONLY);
		if (-1 == res)
			return -errno;
		else
		{
			pread(res, &(stbuf->st_size), sizeof(off_t), 0);
			close(res);
		}
	}

	return 0;
}

static int fuse_fgetattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
	int res;
	res = fstat(file_descriptor(fi), stbuf);
	if (res == -1)
		return -errno;
	if (stbuf->st_mode & S_IFREG)
		stbuf->st_size = file_st_size(fi);
	return 0;
}
/**
 * @func equal to the acccess system call
 * @param path of the file
 * @param mask
 * @return 0 if successful or other if noe
 */
static int fuse_access(const char *path, int mask)
{
	int res;

	res = access(path, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_readlink(const char *path, char *buf, size_t size)
{
	int res;

	res = readlink(path, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}

static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int fuse_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_mkdir(const char *path, mode_t mode)
{
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_rmdir(const char *path)
{
	int res;

	res = rmdir(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_chmod(const char *path, mode_t mode)
{
	int res;

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;

	res = lchown(path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * @func redesign the truncate system call using cas
 * @param path of the file
 * @param the size of the file after truncated
 * @return 0 if sucessful or errno if not
 */
static int fuse_truncate(const char *path, off_t size)
{
	int fd, res = 0;
	off_t st_size = 0;
	off_t i = (size & (g_block_size - 1)) ?
						   size + g_block_size - (size & (g_block_size - 1)) : size;
	unsigned char hash_key[hash_size] = {0};

	fd = open(path, O_RDWR);
	if (fd == -1)
	{
		return -errno;
	}
	res = pread(fd, &st_size, sizeof(off_t), 0);
	if(res == -1)
	{
		return -errno;
	}
	while (i < st_size)
	{
		pread(fd, hash_key, hash_size, i / g_block_size * hash_size + init_offset);
		del_chunk(hash_key, &cfi);
		i += g_block_size;
	}
	res = pwrite(fd, &size, sizeof(off_t), 0);
	close(fd);
	return 0;
}

static int fuse_ftruncate(const char *path, off_t size, struct fuse_file_info *fi)
{
	int fd, res;
	off_t st_size, i;
	unsigned char hash_key[hash_size] = {0};

	i = (size & (g_block_size - 1)) ?
		size + g_block_size - (size & (g_block_size - 1)) : size;
	fd = file_descriptor(fi);
	st_size = file_st_size(fi);

	while (i < st_size) {
		pread(fd, hash_key, hash_size, i / g_block_size * hash_size + init_offset);
		del_chunk(hash_key, &cfi);
		i += g_block_size;
	}

	res = pwrite(fd, &size, sizeof(off_t), 0);
	file_st_size(fi) = size;
	return 0;
}

/**
 * @func redesign the unlink system call, when unlinking a file, first we shoule
 *			truncate its size to 0, and then delete it
 * @param path of the file to be unlinked
 * @return 0 if successful or errno if not
 */
static int fuse_unlink(const char *path)
{
	int res;
	fuse_truncate(path, 0);
	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(path, tv);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * @func redesign of the creat system call, we use the 16 bytes of the 
 *			beginning to store file size infomation
 * @param path of the file to be created
 * @param create mode
 * @param contains infomation of the file to be created like flags
 * @return 0 if successful or errno if not
 */
static int fuse_create(const char *path, mode_t mode, struct fuse_file_info* fi)
{
	off_t st_size[2] = {0};
	int fd = open(path, O_CREAT | O_RDWR | O_EXCL, mode);
	if(fd != -1) {
		fi->fh = alloc_file_context();
		file_descriptor(fi) = fd;
	}
	else {
		return -errno;
	}
	file_st_size(fi) = 0;
	pwrite(fd, st_size, 2 * sizeof(off_t), 0);
	return 0;
}

/**
 * @func redesign of the open system call
 * @param path of the file to be opened
 * @param contains info of the file like open flags
 * @return 0 if successful or errno if not
 */
static int fuse_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	int flags = fi->flags;

	flags &= ~O_WRONLY;
	flags |= O_RDWR;
	flags &= ~O_APPEND;

	res = open(path, flags);
	if (res == -1)
		return -errno;
	fi->fh = alloc_file_context();
	file_descriptor(fi) = res;
	pread(res, &file_st_size(fi), sizeof(off_t), 0);

	return 0;
}

/**
 * @func redesign of the read system call usnig CAS
 * @param path of the file
 * @param buffer where the data should be put into
 * @param size of the bytes to be readed
 * @param offset where the data should be readed from the beginning of the file
 * @param contains file info like file descriptor
 * @return the actual count of bytes read
 */
static int fuse_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int		fd, err = 0;
	int		rdcount = 0;
	off_t	i = offset, st_size = 0;

	unsigned char hash_key[hash_size];


	fd = file_descriptor(fi);		//get file descriptor stored in fuse_file_info

//	read_lock_metafile(fd, offset, size);

	st_size = file_st_size(fi);
	
	st_size = st_size < offset + size ? st_size : offset + size;

	while (i < st_size) {
		
		err = pread(fd, hash_key, hash_size, i / g_block_size * hash_size + init_offset);
		
		if(err == -1) {
			goto read_err;
			//return -errno;
		}
		
		int ret = cache_read(hash_key, buf + i - offset, get_min(g_block_size, size - i + offset));
		if(ret == 0) {
			unsigned char hash[hash_size];
			memcpy(hash,hash_key,hash_size);
			
			rdcount += block_read(hash_key, buf + i - offset, 
					get_min(g_block_size, size - i + offset), 
					i & (g_block_size - 1), &cfi);

			cache_write(hash, buf + i - offset, get_min(g_block_size, size - i + offset));
		} else {
			rdcount += ret;
		}
		i += get_min(size + offset - i, g_block_size - (i & (g_block_size - 1)));
	}

//	read_unlock_metafile(fd, offset, size);

	return rdcount;

read_err:
//	read_unlock_metafile(fd, offset, size);
	return -errno;
}

/**
 * @func redesign the write system call using CAS
 * @param path of the file
 * @param contains data to be writen to the file
 * @param size of the buffer
 * @param offset where the writes begins from the beginning of the file
 * @param contains file info like file descriptor
 * @return the actual count of bytes writen to the file or errno if error occurs
 */
static int fuse_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int		fd = -1;
	int		res = 0;
	long	size_to_write = 0;
	off_t	st_size = 0, i = offset;

	unsigned char data[BLOCK_SIZE], hash_key[hash_size];

	fd = file_descriptor(fi);

//	write_lock_metafile(fd, offset, size);

	st_size = file_st_size(fi);

	while (i < offset + size) {

		memset(data, 0, sizeof(data));
		size_to_write = get_min((long) (size + offset - i), g_block_size - (i & (g_block_size - 1)));

		res = pread(fd, hash_key, hash_size,
				i / g_block_size * hash_size + init_offset);

		if (size_to_write != g_block_size) {
			if (-1 == block_read(hash_key, (char *)data, g_block_size, 0, &cfi))
				printf("[WRITE] block read error!");
		}

		del_chunk(hash_key, &cfi);
		memcpy(data + (i & (g_block_size - 1)), buf + i - offset, size_to_write);

		do_hash(data, g_block_size, hash_key);
		
		res = pwrite(fd, hash_key, hash_size, i / g_block_size * hash_size + init_offset);

		if (res != hash_size) {
			printf("write metafile error at %ld\n", (long) i);
			//return -1;
			goto write_err;
		}
		unsigned char hash[hash_size];

		res = put_new_chunk(hash_key, (char *)data, g_block_size, &cfi);
		if (res) {
			printf("[WRITE] put new chunk error!");
			goto write_err;
		}
		cache_write(hash,data,g_block_size);
		i += size_to_write;
	}

	//update file size
	file_st_size(fi) = st_size < offset + size ? offset + size : st_size;

//	write_unlock_metafile(fd, offset, size);

	return size;
write_err:
//	write_unlock_metafile(fd, offset, size);
	return -errno;
}

/**
 *
 */

static int fuse_statfs(const char *path, struct statvfs *stbuf)
{
	int res;

	res = statvfs(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int fuse_flush(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

/**
 * @func
 * @param
 * @param
 * @return
 */
static int fuse_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	int ret = 0;
	int fd = file_descriptor(fi);
	off_t st_size = file_st_size(fi);
	ret = pwrite(fd, &st_size, sizeof(st_size), 0);
	if (ret < 0) {
		printf("[ERROR]update file size error: ");
		return -errno;
	}
	sync_chunk_file(&cfi);
	close(fd);
	free((struct file_context *)fi->fh);
	return 0;
}

static int fuse_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	int ret = 0;
	pwrite(file_descriptor(fi), &file_st_size(fi), sizeof(off_t), 0);
	if (isdatasync)
		ret = fdatasync(file_descriptor(fi));
	else
		ret = fsync(file_descriptor(fi));
	//sync_chunk_file(&cfi);
	if (ret == -1)
		return -errno;
	else
		return 0;
}

/**
 * @func ioctl system call, the ioctl call is important to the virtual machine, 
 *			so it must be implemented
 * @param path of the file
 * @param cmd to passed to the kernel module
 * @param extra infomation to be passed to the kernel module
 * @param contains the file info like file descriptor
 * @param flag
 * @param data
 * @return ioctl
 */
static int fuse_ioctl(const char *path, int cmd, void *arg, struct fuse_file_info *fi,
						unsigned int flag, void *data)
{
	int ret = ioctl(file_descriptor(fi), cmd, arg, data);
	if (ret == -1)
		return -errno;
	else
		return 0;
}

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int fuse_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int fuse_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int fuse_listxattr(const char *path, char *list, size_t size)
{
	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int fuse_removexattr(const char *path, const char *name)
{
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations fuse_oper = {
	.getattr	= fuse_getattr,
	.fgetattr	= fuse_fgetattr,
	.access		= fuse_access,
	.readlink	= fuse_readlink,
	.readdir	= fuse_readdir,
	.mknod		= fuse_mknod,
	.mkdir		= fuse_mkdir,
	.symlink	= fuse_symlink,
	.unlink		= fuse_unlink,
	.rmdir		= fuse_rmdir,
	.rename		= fuse_rename,
	.link		= fuse_link,
	.chmod		= fuse_chmod,
	.chown		= fuse_chown,
	.truncate	= fuse_truncate,
	.ftruncate	= fuse_ftruncate,
	.utimens	= fuse_utimens,
	.create		= fuse_create,
	.open		= fuse_open,
	.read		= fuse_read,
	.write		= fuse_write,

	.statfs		= fuse_statfs,
	.flush		= fuse_flush,
	.release	= fuse_release,
	.fsync		= fuse_fsync,
	.ioctl		= fuse_ioctl,
#ifdef HAVE_SETXATTR
	.setxattr	= fuse_setxattr,
	.getxattr	= fuse_getxattr,
	.listxattr	= fuse_listxattr,
	.removexattr	= fuse_removexattr,
#endif
};


int main(int argc, char *argv[])
{
#if defined (__DEBUG_OUTPUT)
	freopen("/root/log_err.txt","w+",stderr);
	freopen("/root/log_out.txt","w+",stdout);
#endif
	umask(0);
	int ret = 0;

	ret = meta_server_init("/root/agilefs-hash.db");
	if (ret != 0)
		goto meta_server_init_err;

	ret = init_chunk_file(&cfi, "/root/.agilefs.conf");
	if (ret != 0)	
		goto server_init_err;

#if FUSE_VERSION >= 26
	ret = fuse_main(argc, argv, &fuse_oper, NULL);
#else
	ret = fuse_main(argc, argc, &fuse_oper);
#endif

server_init_err:	
	release_chunk_file(&cfi, "/root/.agilefs.conf");
meta_server_init_err:
	meta_server_close();
	return 0;
}
