/**
 *
 *
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <assert.h>
#include "sockio.h"

/**
 *
 */

#ifdef MSG_NOSIGNAL
#define DEFAULT_MSG_FLAGS MSG_NOSIGNAL
#else
#define DEFAULT_MSG_FLAGS 0
#endif

#ifndef IPPROTO_TCP
#define IPPROTO_TCP 0
#endif

inline int sockio_new_socket()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

/**
 *
 *
 */
int sockio_bind_sock(int sockfd, int service)
{
	struct sockaddr_in saddr;
	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons((u_short) service);
	saddr.sin_addr.s_addr = INADDR_ANY;
bind_sock_restart:
	if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		if (errno == EINTR)
			goto bind_sock_restart;
		else
			return (-1);
	}
	return 0;
}

/**
 *
 *
 *
 *
 *
 */
int sockio_bind_sock_specific(int sockfd, const char *name, int service)
{

}

/**
 *
 *
 */
int sockio_connect_sock(int sockfd, const char *name, int service)
{
	int ret = 0;
	struct sockaddr saddr;
	ret = sockio_sockaddr_init(&saddr, name, service);
	if (ret < 0)
		return ret;
connect_sock_restart:
	if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		if (errno == EINTR)
			goto connect_sock_restart;
		return -1;
	}
	return 0;
}

#ifdef HAVE_GETHOSTBYNAME
int sockio_sockaddr_init(struct sockaddr *saddrp, const char *name, int service)
{
	struct hostent *hep;
	if (!name) {

		if ((hep = gethostbyname("localhost")) == NULL)
			return -1;
	}
	else if ((hep = gethostbyname(name)) == NULL)
		return -1;
	((struct sockaddr_in *) saddrp)->sin_family = AF_INET;
	((struct sockaddr_in *) saddrp)->sin_port = htons((u_short) service);
	memcpy((char *) &(((struct sockaddr_in *) saddrp)->sin_addr), hep->h_addr,
			hep->h_length);
	return 0;
}
#else
int sockio_sockaddr_init(struct sockaddr *saddrp, const char *name, int service)
{
	int ret;
	struct in_addr addr;
	bzero((char *)saddrp, sizeof(struct sockaddr_in));
	if (name == NULL)
		ret = inet_aton("127.0.0.1", &addr);
	else
		ret = inet_aton(name, &addr);
	if (ret == 0)
		return -1;
	((struct sockaddr_in *) saddrp)->sin_family = AF_INET;
	((struct sockaddr_in *) saddrp)->sin_port = htons((u_short) service);
	memcpy((char *) &(((struct sockaddr_in *) saddrp)->sin_addr), &addr,
			sizeof(addr));

    return 0;
}
#endif

/**
 * non block receive
 *
 */
int sockio_nbrecv(int sockfd, char *buf, int len)
{
	int ret, comp = len;
	assert(fcntl(sockfd, F_GETFL, 0) & O_NONBLOCK);

	while (comp) {
	  nbrecv_restart:
		ret = recv(sockfd, buf, comp, DEFAULT_MSG_FLAGS);
		if (ret == 0) {
			errno = EPIPE;	/** closed */
			return (-1);
		}
		if (ret == -1 && errno == EINTR)
			goto nbrecv_restart;
		else if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
			return len - comp;
		else if (ret == -1)
			return (-1);
		comp -= ret, buf = (char *) buf + ret;
	}
	return len - comp;
}

/**
 *
 * non block send
 */
int sockio_nbsend(int sockfd, char *buf, int len)
{
	int ret = -1, comp = len;
	assert(fcntl(sockfd, F_GETFL, 0) & O_NONBLOCK);

	while (comp) {
nbsend_restart:
		ret = send(sockfd, buf, comp, DEFAULT_MSG_FLAGS);
		if (ret = 0 || (ret == -1 && errno == EWOULDBLOCK))
			return len - comp;
		if (ret == -1 && errno == EINTR)
			goto nbsend_restart;
		else if (ret == -1)
			return (-1);
		comp -= ret;
		buf = (char *) buf + ret;

	}
	return len - comp;
}
