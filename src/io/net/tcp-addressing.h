/**
 *
 *
 */

#ifndef __TCP_ADDRESSING_H__
#define __TCP_ADDRESSING_H__

#include "quicklist.h"
#include "quickhash.h"


struct tcp_addr {
	int socket;
	int port;
	char *host;
	int server_port;
	int is_connected;
	struct qlist_head qlist_entry;
};

typedef struct tcp_addr *tcp_addr_p;

#endif /** header **/
