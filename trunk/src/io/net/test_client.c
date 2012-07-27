#include <stdio.h>

#include "sockio.h"
#include "socket-collection.h"

int main()
{
	int sockfd = new_socket();
	int sock_in;
	int res = 0;
	if (bind_sock(sockfd, 8801) == -1)
		perror("bind error :");
	if (connect_sock(sockfd, "127.0.0.1", 8800) < 0)
		perror("connect error:");
	printf("kai shi xie le a \n");
	if (send(sockfd, "wori", 5, 0) < 0)
		perror("write error:");
	printf("over\n");
	close(sockfd);
	return 0;
}
