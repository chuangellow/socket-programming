#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(void) {
	char *ip = "0.0.0.0";
	in_addr_t addr;
	if ((addr = inet_addr(ip)) == INADDR_NONE) {
		perror("inet_addr");
		exit(1);
	}
	printf("inet_addr: %s\n", ip);
	printf("inet_addr: %u\n", ntohl(addr));

	char *invalid_ip = "0.0.0.256";
	if ((addr = inet_network(invalid_ip)) == INADDR_NONE) {
		perror("inet_network");
		exit(1);
	}
	return 0;
}
