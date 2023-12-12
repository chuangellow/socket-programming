#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(void) {
	int sock_fd;
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
	};

	if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}

	char buf[BUFFER_SIZE];

	while (1) {
		printf("Enter a message: ");
		fgets(buf, BUFFER_SIZE, stdin);
		if (send(sock_fd, buf, strlen(buf), 0) < 0) {
			perror("send");
			exit(EXIT_FAILURE);
		}
		if (recv(sock_fd, buf, BUFFER_SIZE, 0) < 0) {
			perror("recv");
			exit(EXIT_FAILURE);
		}
		printf("Server: %s\n", buf);

		if (strncmp(buf, "exit", 4) == 0) {
			break;
		}

		memset(buf, 0, BUFFER_SIZE);
	}
	return 0;
}
