/**
 *
 *
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "sockio.h"
#include "socket-collection.h"
#include "tcp.h"

char *srv_ip_list[] = {"192.168.5.74", "192.168.5.83"};
short client_ports[] = {8000, 8001, 8002};

static struct socket_collection *scp = NULL;

struct tcp_addr server_tcp_addr;

static int tcp_server_init();

int tcp_initialize(tcp_addr_p listen_addr, int flags)
{
	int retval = 0;
	if (flags & TCP_SERVER_INIT) {
		retval = tcp_server_init();
		if (retval < 0) {
			fprintf(stderr, "tcp_initialize failed: %s\n", strerror(retval));
			return retval;
		}
		scp = socket_collection_init(listen_addr->socket);
		if (!scp)
			return (-1);
	} else {
		scp = socket_collection_init(-1);
	}

}

/**
 * tcp intialize on server side
 * return 0 if sucess else -errno
 */
static int tcp_server_init()
{
	int oldfl, ret;
	struct tcp_addr *srv_tcp_addrp = &server_tcp_addr;
	memset(srv_tcp_addrp, 0, sizeof(struct tcp_addr));
	srv_tcp_addrp->socket = sockio_new_socket();
	if (srv_tcp_addrp->socket < 0)
		return -errno;

	oldfl = fcntl(srv_tcp_addrp->socket, F_GETFL, 0);
	if (!(oldfl & O_NONBLOCK))
		fcntl(srv_tcp_addrp->socket, F_SETFL, oldfl | O_NONBLOCK);
	
	sockio_set_sockopt(srv_tcp_addrp->socket, SO_REUSEADDR, 1);
	
	ret = sockio_bind_sock(srv_tcp_addrp->socket, srv_tcp_addr->port);
	if (ret < 0)
		return -errno;
	if (listen(srv_tcp_addrp->socket, 256) != 0)
		return -errno;
	return 0;

}

static int tcp_client_init()
{
	return (0);
}
