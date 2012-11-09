/**
 * file:	tcp.c
 * author:	jarvis
 * date:	2012-10-04
 * description:
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>

#include "sockio.h"
#include "socket-collection.h"
#include "tcp.h"

char *srv_ip_list[] = {"192.168.5.74", "192.168.5.83"};
short client_ports[] = {8000, 8001, 8002};

static struct socket_collection *scp = NULL;

struct {
	int method_flags;
	tcp_addr_p listen_addr;
} tcp_method_params;

static int tcp_server_init();
static int tcp_sock_init(tcp_addr_p addrp);
static int tcp_accept_init(int *socket, char **peer);

int tcp_initialize(tcp_addr_p listen_addr, int init_flags)
{
	int retval = 0;
	
	memset(&tcp_method_params, 0, sizeof(tcp_method_params));
	tcp_method_params.method_flags = init_flags;

	if (init_flags & TCP_SERVER_INIT) {

		if (!listen_addr)
			listen_addr = (tcp_addr_p)malloc(sizeof(struct tcp_addr));

		tcp_method_params.listen_addr = listen_addr;
		retval = tcp_server_init();
		if (retval < 0) {
			fprintf(stderr, "tcp_initialize failed: %s\n", strerror(retval));
			return retval;
		}
		scp = socket_collection_init(listen_addr->socket);
	} else {
		scp = socket_collection_init(-1);
	}

	if (!scp)
		return (-1);
	return (0);

}

/**
 * tcp intialize on server side
 * return 0 if sucess else -errno
 */
static int tcp_server_init()
{
	int oldfl, ret;
	struct tcp_addr *srv_tcp_addrp = tcp_method_params.listen_addr;
	memset(srv_tcp_addrp, 0, sizeof(struct tcp_addr));
	srv_tcp_addrp->socket = sockio_new_socket();
	if (srv_tcp_addrp->socket < 0)
		return -errno;

	oldfl = fcntl(srv_tcp_addrp->socket, F_GETFL, 0);
	if (!(oldfl & O_NONBLOCK))
		fcntl(srv_tcp_addrp->socket, F_SETFL, oldfl | O_NONBLOCK);
	
	sockio_set_sockopt(srv_tcp_addrp->socket, SO_REUSEADDR, 1);		//set reuse addr
	
	ret = sockio_bind_sock(srv_tcp_addrp->socket, srv_tcp_addrp->port);	//bind sock on the server side
	if (ret < 0)
		return -errno;
	if (listen(srv_tcp_addrp->socket, 256) != 0)		//start listening
		return -errno;
	return 0;
}

/**
 *
 * client side initialize
 *
 */
static int tcp_sock_init(tcp_addr_p addrp)
{
	int oldfl = 0, ret = 0;

	assert(addrp->server_port == 0);

	if (addrp->socket == -1) {
		addrp->socket = sockio_new_socket();

		oldfl = fcntl(addrp->socket, F_GETFL, 0);
		if (!(oldfl & O_NONBLOCK))
			fcntl(addrp->socket, F_SETFL, oldfl | O_NONBLOCK);

		if (sockio_set_tcpopt(addrp->socket, TCP_NODELAY, 1) < 0) {
			close(addrp->socket);
			return (-1);
		}

		if (addrp->hostname) {
			ret = sockio_connect_sock(addrp->socket, addrp->hostname, addrp->port);
			addrp->is_connected = 1;
		}
	} 

	return (0);
}

int tcp_accept_init(int *socket, char **peer)
{
	int ret = 0, oldfl = 0;
	struct sockaddr_in peer_sockaddr;
	int peer_addr_size = sizeof(struct sockaddr_in);
	tcp_addr_p srv_addrp = tcp_method_params.listen_addr;

	ret = accept(srv_addrp->socket, (struct sockaddr *)&peer_sockaddr, (socklen_t *)&peer_addr_size);
	if (ret < 0)
		return (-1);
	*socket = ret;

	if (sockio_set_tcpopt(*socket, TCP_NODELAY, 1) < 0) {	// turn off nagile algorithm
		perror("set tcp option failed: ");
		close(*socket);
		return (-1);
	}

	oldfl = fcntl(*socket, F_GETFL, 0);
	if (!(oldfl & O_NONBLOCK)) {
		fcntl(*socket, F_SETFL, oldfl | O_NONBLOCK);	// set to non-block mode
	}

	return (0);
}

int tcp_send_generic(tcp_addr_p addrp, void *buffer, int len)
{

	int ret = 0;
	if (!addrp->is_connected) {
		ret = tcp_sock_init(addrp);
		if (ret)
			return (-1);
		addrp->is_connected = 1;
//		socket_collection_add(addrp, scp);	//add to socket collection
	}
	return 0;
}
