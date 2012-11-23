/**
 *
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "agilefs-def.h"
#include "direct-ios.h"
#include "space-immu.h"


extern struct storage_operations chunk_ops;

struct space_context *open_context(const char *config_path)
{
	struct space_context *tmp_scp;
	int ret;
	tmp_scp = alloc_space_context();
	if (tmp_scp == NULL) {
		perror("alloc space context struct failed :");
		return NULL;
	}

	tmp_scp->cfip = alloc_chunk_file_info();
	if (!tmp_scp->cfip) {
		perror("");
		free(tmp_scp);
		return NULL;
	}
	ret = init_chunk_file(tmp_scp->cfip, config_path);
	scp->s_op = &chunks_op;

	return tmp_scp;
}

int space_write(struct space_context *scp,
		void *key, const char *buf, size_t size)
{
	int ret = 0;

	assert(scp->s_op);
	if (!key) {
		return (-1);
	}
	ret = scp->s_op->put(key, buf, size, scp->cfip);
	return ret;
}

int space_read(struct space_context *scp,
		void *key, char *buf, size_t size, off_t offset)
{
	int ret = 0;
	return ret;
}

int space_sync_data(struct space_context *scp)
{
	int ret = 0;
	ret = scp->s_op->sync(scp->cfip);
	return ret;
}

int space_close(struct space_context *scp)
{
	int ret = 0;
	return ret;
}
