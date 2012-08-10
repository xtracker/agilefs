/**
 * filename	:	socket-collection.c
 * Author	:	Zhao Guoyu
 *
 *
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "socket-collection.h"

#ifdef __USE_LINUX_EPOLL
#define EPOLL_ERR_MASK (EPOLLERR | EPOLLHUP)
#define EPOLL_CREATE_SIZE 32
#endif

/**
 *
 * return NULL if fails or the pointer to socket_collection
 */
socket_collection_p socket_collection_init(int server_socket)
{
	int ret = 0;
	struct epoll_event event;
	socket_collection_p tscp = (struct socket_collection *) malloc(sizeof(struct socket_collection));
	if (!tscp)
		return NULL;
	memset(tscp, 0, sizeof(struct socket_collection));

	tscp->epfd = epoll_create(EPOLL_CREATE_SIZE);
	if (tscp->epfd < 0) {
		perror("epoll create error");
		free(tscp);
		return NULL;
	}
	tscp->server_socket = server_socket;
	if (server_socket > -1) {
		memset(&event, 0, sizeof(event));
		event.events = (EPOLLIN | EPOLLOUT | EPOLLHUP);
		event.data.ptr = NULL;
		ret = epoll_ctl(tscp->epfd, EPOLL_CTL_ADD, server_socket, &event);
		if (ret < 0 && errno != EEXIST) {
			perror("epoll control add failure:");
			free(tscp);
			return NULL;
		}

	}
	return tscp;
}

void socket_collection_finalize(socket_collection_p scp)
{
	assert(scp != NULL);
	free(scp);
	return;
}

int socket_collection_testglobal(socket_collection_p scp,
		int incount,
		int *outcount,
		int epoll_timeout)
{
	int ret;
	int tmp_count;
	
	if (incount == 0)
		return 0;

	if (incount > EPOLL_MAX_PER_CYCLE)
		incount = EPOLL_MAX_PER_CYCLE;

	do {
		ret = epoll_wait(scp->epfd, scp->event_array, incount, epoll_timeout);
	} while (ret < 0 && errno == EINTR);

	if (ret == 0)
		return 0;

	tmp_count = ret;
	return 0;

}
