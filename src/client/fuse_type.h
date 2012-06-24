/*
 * fuse_type.h
 *
 *  Created on: May 29, 2011
 *      Author: jarvis
 */

#ifndef FUSE_TYPE_H_
#define FUSE_TYPE_H_


typedef unsigned char u_byte;				//byte

/**
 * metafile header struct
 *
 */
struct fuse_metafile_header {
	off_t __fsize[2];			//size of the orginal file
};

inline void do_hash(void *input, int length, void *output)
{
#ifdef __USE_SHA1
	SHA1((const unsigned char *)input, length, output);
#else
	md5((unsigned char *)input, length, output);
#endif
}

#endif /* FUSE_TEST_H_ */

