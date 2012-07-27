#include <stdio.h>

#include "sockio.h"
#include "socket-collection.h"

int main()
{
	int sockfd = new_socket();
	int len;
	struct sockaddr saddr;
	int sock_in;
	char buff[4096] = {0};
	if (bind_sock(sockfd, 8800) < 0)
		perror("bind error");
	if (listen(sockfd, 5) < 0)
		perror("listen error");
	while (1) {
		printf("wogan\n");
		sock_in = accept(sockfd, &saddr, &len);
		read(sock_in, buff, 4096);
		printf("%s\n",buff);
		fflush(stdout);
	}
	return 0;
}
