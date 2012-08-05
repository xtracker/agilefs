/**
 *
 *
 */

#ifndef __SOCK_IO_H__
#define __SOCK_IO_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

int sockio_new_sock();
int sockio_bind_sock(int sockfd,
		int service);
int sockio_connect_sock(int sockfd,
		const char *name,
		int service);
int sockio_sockaddr_init(struct sockaddr *saddrp,
		const char *name,
		int service);
int sockio_nbrecv(int sockfd,
		char *buf,
		int len);
int sockio_nbvector(int sockfd,
		struct iovec * vector,
		int count,
		int recv_flag);

int sockio_get_sockopt(int sockfd, int optname);
int sockio_set_tcpopt(int sockfd,
		int optname,
		int val);
int sockio_set_sockopt(int sockfd,
		int optname,
		int val);

#define SET_NONBLOCK(x_fd) fcntl((x_fd), F_SETFL, O_NONBLOCK | \
		fcntl((x_fd), F_GETFL, 0))

#endif /**/

