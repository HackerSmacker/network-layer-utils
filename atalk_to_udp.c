#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <netatalk/at.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1316
#define DDP_MAX_DATA_SIZE 586

int main(int argc, char** argv) {
	int udp_sock, ddp_sock, ip_port;
	char* ip_addr;
	unsigned char atalk_port, atalk_host;
	unsigned short atalk_net;
	struct sockaddr_in udp_addr;
	char udp_buffer[BUFFER_SIZE];
	struct sockaddr_at ddp_addr;
	char buffer[BUFFER_SIZE];
	int total_received = 0;
	ssize_t recv_len, sent_len;
	
	if(argc != 6) {
		printf("Usage: %s <ATalk net> <ATalk host> <ATalk port> <destination IP> <destination UDP port>\n", argv[0]);
	printf("To bind any interface, use 0 for the AppleTalk net and host.\n");
		exit(1);
	}
	atalk_net = atoi(argv[1]);
	atalk_host = atoi(argv[2]);
	atalk_port = atoi(argv[3]);
	ip_addr = argv[4];
	ip_port = atoi(argv[5]);

	/* DDP */
	ddp_sock = socket(AF_APPLETALK, SOCK_DGRAM, 0);
	if(ddp_sock < 0) {
		perror("DDP: socket: ");
		close(udp_sock);
		exit(-1);
	}

	memset(&ddp_addr, 0, sizeof(ddp_addr));
	ddp_addr.sat_family = AF_APPLETALK;
	ddp_addr.sat_port = atalk_port;
	ddp_addr.sat_addr.s_net = atalk_net; 
	ddp_addr.sat_addr.s_node = atalk_host;
	
	if(bind(ddp_sock, (struct sockaddr*) &ddp_addr, sizeof(ddp_addr)) < 0) {
		perror("DDP: bind: ");
		close(ddp_sock);
		exit(-1);
	}

	/* UDP */
	udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_sock < 0) {
		perror("UDP: socket: ");
		exit(-1);
	}

	memset(&udp_addr, 0, sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	udp_addr.sin_port = htons(ip_port);

	printf("Listening for DDP messages on %04x:%02x.%02x...\n", atalk_net, atalk_host, atalk_port);
	printf("Destination address is %s:%d\n", ip_addr, ip_port);

	while(1) {
		recv_len = recvfrom(ddp_sock, buffer + total_received, BUFFER_SIZE - total_received, 0, NULL, NULL);
		if(recv_len < 0) {
			perror("DDP: recvfrom: ");
			continue;
		}

		total_received += recv_len;

		// Process data in chunks of DDP_MAX_DATA_SIZE
		while(total_received > 0) {
			// Determine how much data to send: max DDP_MAX_DATA_SIZE - 1 (for the port byte)
			int send_size = (total_received > DDP_MAX_DATA_SIZE - 1) ? DDP_MAX_DATA_SIZE - 1 : total_received;

			// Create the packet with the first byte skipped over
			unsigned char packet[DDP_MAX_DATA_SIZE];
			memcpy(packet + 1, buffer, send_size);

			// Send the packet
			sent_len = sendto(udp_sock, packet, send_size + 1, 0, (struct sockaddr*) &udp_addr, sizeof(udp_addr));
			if(sent_len < 0) {
				perror("UDP: sendto: ");
			}

			// Shift remaining data
			total_received -= send_size;
			memmove(buffer, buffer + send_size, total_received); // Move the remaining data to the front

			// If we sent the last of the received data, break
			if(total_received == 0) {
				break;
			}
		}
	}

	close(udp_sock);
	close(ddp_sock);
	return 0;
}

