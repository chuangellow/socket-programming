#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main(void) {
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	printf("socket_fd = %d\n", socket_fd);
	return 0;
}
