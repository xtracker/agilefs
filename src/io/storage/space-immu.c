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
#include "space-immu.h"


struct space_context *open_context()
{
	struct space_context *tmp_scp;
	tmp_scp = (struct space_context *) malloc(sizeof(struct space_context));
	if (tmp_scp == NULL) {
		perror("open context failed :");
		return NULL;
	}
	return tmp_scp;
}
