/**
 *
 *
 */

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "chunks.h"
#include "chunk-io.h"
#include "db_ops.h"

struct space_context {
	struct chunk_file_info *cfip;

};

typedef struct space_context *space_context_p;

struct space_context *open_context();

int space_write(struct space_context *scp,
		const char *buf,
		size_t size);

int space_read(struct sapce_context *scp,
		char *buf,
		size_t size);

int close(struct space_context *scp);

#endif
