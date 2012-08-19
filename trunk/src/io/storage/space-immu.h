/**
 *
 *
 */

#ifndef __SPACE_IMMU_H__
#define __SPACE_IMMU_H__

#include "chunks.h"
#include "chunk-io.h"
#include "db_ops.h"

extern struct chunk_file_info cfi;


struct space_context {
	struct chunk_file_info *cfip;
	union {
		unsigned int bitmap;
		struct {
			unsigned int bits : 1[32];
		};
	}fd_bitmap;

};

typedef struct space_context *space_context_p;

struct space_context *space_open();

int space_write(struct space_context *scp,
		const char *buf,
		size_t size);

int space_read(struct sapce_context *scp,
		char *buf,
		size_t size);

int space_close(struct space_context *scp);

#endif
