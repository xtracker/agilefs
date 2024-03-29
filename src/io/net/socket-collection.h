/**
 * file:	socket-collection.h
 * author:	zhaoguoyu
 * date:	2012-06-01
 * header file of socket collections
 */

#ifndef __SOCKET_COLLECTION_H__
#define __SOCKET_COLLECTION_H__

#include <sys/poll.h>
#include <sys/epoll.h>

#define EPOLL_MAX_PER_CYCLE 16

struct socket_collection {
	int server_socket;
	int epfd;
	struct epoll_event event_array[EPOLL_MAX_PER_CYCLE];
};

typedef struct socket_collection *socket_collection_p;

socket_collection_p socket_collection_init(int server_socket);
void socket_collection_finalize(socket_collection_p scp);

#define socket_collection_add(m, s)	\
	do { \
		struct tcp_addr* tcp_data = (m); \
		if(tcp_data->socket > -1){ \
			struct epoll_event event;\
			memset(&event, 0, sizeof(event));\
			event.events = EPOLLIN|EPOLLERR|EPOLLHUP;\
			event.data.ptr = NULL;\
			epoll_ctl(s->epfd, EPOLL_CTL_ADD, tcp_data->socket, &event);\
		}\
	} while (0);

#define socket_collection_remove(m, s) \
	do { \
	} while (0);


#endif	/* __SOCKET_COLLECTION_H__ */
