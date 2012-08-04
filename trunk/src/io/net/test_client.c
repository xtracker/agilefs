#include <stdio.h>

#include "sockio.h"
#include "socket-collection.h"

char buffer[4096];
int main()
{
	int sockfd = sockio_new_socket();
	int sock_in;
	int res = 0;
	if (sockio_bind_sock(sockfd, 8801) == -1)
		perror("bind error :");
	if (sockio_connect_sock(sockfd, "192.168.5.74", 8800) < 0)
		perror("connect error:");
	printf("kai shi xie le a \n");
	
	while (scanf("%s", buffer) != EOF) {
		if (send(sockfd, buffer, strlen(buffer), 0) < 0)
			perror("write error:");
	}
	printf("over\n");
	close(sockfd);
	return 0;
}
