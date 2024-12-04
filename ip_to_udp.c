#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/ip.h>

#define BUFFER_SIZE 1316 /* Packet payload size */

int main(int argc, char** argv) {
	int udp_sock, raw_sock, udp_port, ip_proto;
	char* udp_addr_str;
	struct sockaddr_in udp_addr;
	char buffer[BUFFER_SIZE + 20];
	ssize_t recv_len, sent_len;

	if(argc != 4) {
		printf("Usage: %s <IP protocol> <UDP destination IP> <UDP destination port>\n", argv[0]);
		exit(-1);
	}

	ip_proto = atoi(argv[1]);
	udp_addr_str = argv[2];
	udp_port = atoi(argv[3]);

	raw_sock = socket(PF_INET, SOCK_RAW, ip_proto);
	if(raw_sock < 0) {
		perror("Raw socket: ");
		exit(-1);
	}

	udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_sock < 0) {
		perror("UDP socket: ");
		close(raw_sock);
		exit(-1);
	}

	memset(&udp_addr, 0, sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(udp_port);
	udp_addr.sin_addr.s_addr = inet_addr(udp_addr_str);

	printf("Listening for raw packets on protocol %d...\n", ip_proto);
	printf("Sending data to UDP address: %s:%d\n", udp_addr_str, udp_port);

	while(1) {
		recv_len = recv(raw_sock, buffer, sizeof(buffer), 0);
		if(recv_len < 0) {
			perror("RAW: recv: ");
			continue;
		}

		sent_len = sendto(udp_sock, buffer, recv_len, 0, (struct sockaddr*) &udp_addr, sizeof(udp_addr));
		if(sent_len < 0) {
			perror("UDP sendto: ");
		}
	}

	close(udp_sock);
	close(raw_sock);
	return 0;
}

