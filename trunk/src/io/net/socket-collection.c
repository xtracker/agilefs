#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "socket-collection.h"
#ifdef __USE_LINUX_EPOLL
#define EPOOL_CREATE_SIZE 32
#endif

socket_collection_p socket_collection_init(int sockfd)
{
	return NULL;
}
