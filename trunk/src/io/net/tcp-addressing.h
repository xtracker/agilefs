/**
 *
 *
 */

#ifndef __TCP_ADDRESSING_H__
#define __TCP_ADDRESSING_H__

struct tcp_addr {
	int sockfd;
	int port;
	char *host;
};

typedef struct tcp_addr *tcp_addr_p;

#endif /** header **/
