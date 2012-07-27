/**
 *
 *
 */

#ifndef __SOCKET_COLLECTION_H__
#define __SOCKET_COLLECTION_H__

#include <sys/poll.h>
#include <sys/epoll.h>


struct socket_collection {
	int epfd;
	int server_socket;
};

typedef struct socket_collection *socket_collection_p;

socket_collection_p socket_collection_init(int sockfd);

#define socket_collection_add(m, s)	\
	do {	\
	} while (0);	\

#endif	/* __SOCKET_COLLECTION_H__ */
