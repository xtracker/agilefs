/**
 *
 *
 */

#ifndef __SOCKET_COLLECTION_H__
#define __SOCKET_COLLECTION_H__

#include <sys/poll.h>
#include <sys/epoll.h>

#define EPOLL_MAX_PER_CYCLE 16

struct socket_collection {
	int epfd;
	int server_socket;
	struct epoll_event evnet_array[EPOLL_MAX_PER_CYCLE];
};

typedef struct socket_collection *socket_collection_p;

socket_collection_p socket_collection_init(int server_socket);
void socket_collection_finalize(socket_collection_p scp);

#define socket_collection_add(m, s)	\
	do { \
	} while (0);

#define socket_collection_remove(m, s) \
	do { \
	} while (0);


#endif	/* __SOCKET_COLLECTION_H__ */
