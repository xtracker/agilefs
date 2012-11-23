/**
 *
 *
 */

#ifndef __SPACE_IMMU_H__
#define __SPACE_IMMU_H__

#if defined (__cplusplus)
extern "C" {
#endif

#include "chunks.h"
#include "chunks-io.h"
#include "db_ops.h"


#define alloc_space_context() \
	(struct space_context *) malloc(sizeof(struct space_context))

struct storage_operations {
	int (*put)(void *, const char *, size_t, struct chunk_file_info *);
	int (*get)(void *, char *, size_t, off_t , struct chunk_file_info *);
	int (*del)(void *, struct chunk_file_info *);
	int (*sync)(struct chunk_file_info *);
};

struct space_context {
	struct chunk_file_info *cfip;
	const struct storage_operations *s_op;
};

typedef struct space_context *space_context_p;

struct space_context *space_open(const char *);

int space_write(struct space_context *scp,
		void *key,
		const char *buf,
		size_t size);

int space_read(struct space_context *scp,
		void *key,
		char *buf,
		size_t size,
		off_t offset);

int space_close(struct space_context *scp, void *key);

int space_sync_data(struct space_context *scp);

#if defined (__cplusplus)
}
#endif
#endif
