/**
 *
 *
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sockio.h"
#include "socket-collection.h"

static struct socket_collection *scp = NULL;

static int server_socket;

static int tcp_server_init();

int agilefs_tcp_init();

