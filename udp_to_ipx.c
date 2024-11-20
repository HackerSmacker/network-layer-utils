#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <netipx/ipx.h>
#include <unistd.h>

#define BUFFER_SIZE 1316

int main(int argc, char** argv) {
	int udp_sock, ipx_sock, udp_port;
	struct sockaddr_in udp_addr;
	char udp_buffer[BUFFER_SIZE];
	struct sockaddr_ipx ipx_addr;
	unsigned short ipx_port;
	unsigned int ipx_net;
	char* ipx_node;
	char msg[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	int total_received = 0;
	ssize_t recv_len, sent_len;
	int i, j = 0;
	char current_addr_byte[3];

	
	if(argc != 5) {
		printf("Usage: %s <UDP port> <IPX network> <IPX host> <IPX port>\n", argv[0]);
	printf("All IPX numbers are in hex.\n");
	exit(-1);
	}
	udp_port = atoi(argv[1]);
	ipx_net = strtol(argv[2], NULL, 16);
	ipx_node = argv[3];
	ipx_port = strtol(argv[4], NULL, 16);

	udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_sock < 0) {
		perror("UDP: socket: ");
		exit(-1);
	}

	memset(&udp_addr, 0, sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_addr.s_addr = INADDR_ANY;
	udp_addr.sin_port = htons(udp_port);

	if(bind(udp_sock, (struct sockaddr*) &udp_addr, sizeof(udp_addr)) < 0) {
		perror("UDP: bind: ");
		close(udp_sock);
		exit(-1);
	}

	ipx_sock = socket(AF_IPX, SOCK_DGRAM, 0);
	if(ipx_sock < 0) {
		perror("IPX: socket: ");
		close(udp_sock);
		exit(-1);
	}

	memset(&ipx_addr, 0, sizeof(ipx_addr));
	ipx_addr.sipx_family = AF_IPX;
	ipx_addr.sipx_network = ipx_net; /* 0 */
	ipx_addr.sipx_port = htons(ipx_port);
	/*
	ipx_addr.sipx_node[0] = 0xFF;
	ipx_addr.sipx_node[1] = 0xFF;
	ipx_addr.sipx_node[2] = 0xFF;
	ipx_addr.sipx_node[3] = 0xFF;
	ipx_addr.sipx_node[4] = 0xFF;
	ipx_addr.sipx_node[5] = 0xFF;
	*/
	for(i = 0; i < 12; i += 2) {
		current_addr_byte[0] = ipx_node[i];
		current_addr_byte[1] = ipx_node[i + 1];
	current_addr_byte[2] = '\0';
		ipx_addr.sipx_node[j++] = strtol(current_addr_byte, NULL, 16);
	}

	printf("Listening for UDP messages on port %d...\n", udp_port);
	printf("Destination address is %08x.%02x%02x%02x%02x%02x%02x:%04x\n", ipx_addr.sipx_network, ipx_addr.sipx_node[0], 
			ipx_addr.sipx_node[1], ipx_addr.sipx_node[2], ipx_addr.sipx_node[3], 
			ipx_addr.sipx_node[4], ipx_addr.sipx_node[5], ntohs(ipx_addr.sipx_port));

	while(1) {
		recv_len = recvfrom(udp_sock, buffer + total_received, BUFFER_SIZE - total_received, 0, NULL, NULL);
		if(recv_len < 0) {
			perror("UDP: recvfrom: ");
			continue;
		}

		total_received += recv_len;

		if(total_received >= BUFFER_SIZE) {
			sent_len = sendto(ipx_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &ipx_addr, sizeof(ipx_addr));
			if(sent_len < 0) {
				perror("IPX: sendto: ");
			} 
			total_received = 0;
		}
	}

	close(udp_sock);
	close(ipx_sock);
	return 0;
}

