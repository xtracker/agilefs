/**
 *
 */

#ifndef __TCP_H__
#define __TCP_H__

#include "agilefs-def.h"
#include "tcp-addressing.h"

#define TCP_SERVER_INIT 1
#define TCP_CLIENT_INIT 2

enum fs_ops {
	FS_WRITE,
	FS_READ,
	FS_DELETE,
};

struct msg_header {
	enum fs_ops op;
	int d_len;
	char key[HASH_SIZE];
	char data[0];
};

int tcp_initialize(tcp_addr_p listen_addr, int flags);
int tcp_finalize(void);

#endif /* __TCP_H__ */

