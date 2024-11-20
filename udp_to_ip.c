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
	unsigned int src_addr;
	char* ip_addr;
	struct sockaddr_in udp_addr;
	struct sockaddr_in raw_addr;
	char msg[BUFFER_SIZE];
	char buffer[BUFFER_SIZE + 20];
	int total_received = 0;
	ssize_t recv_len, sent_len;
	struct iphdr* ip_hdr = (struct iphdr*) buffer;

	
	if(argc != 5) {
		printf("Usage: %s <UDP port> <source IP host> <IP host> <IP protocol>\n", argv[0]);
	exit(-1);
	}
	udp_port = atoi(argv[1]);
	src_addr = inet_addr(argv[2]);
	ip_addr = argv[3];
	ip_proto = atoi(argv[4]);

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

	raw_sock = socket(PF_INET, SOCK_RAW, ip_proto);
	if(raw_sock < 0) {
		perror("IP: socket: ");
		close(udp_sock);
		exit(-1);
	}

	int one = 1;
	if(setsockopt(raw_sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
		perror("IP: setsockopt: ");
		exit(2);
	}

	memset(&raw_addr, 0, sizeof(raw_addr));
	raw_addr.sin_family = AF_INET;
	raw_addr.sin_addr.s_addr = inet_addr(ip_addr);
	raw_addr.sin_port = 0;
	ip_hdr->ihl = 5;
	ip_hdr->version = 4;
	ip_hdr->tos = 16;
	ip_hdr->tot_len = BUFFER_SIZE + 20;
	ip_hdr->id = htons(54321);
	ip_hdr->ttl = 64;
	ip_hdr->protocol = ip_proto;
	ip_hdr->saddr = src_addr;
	ip_hdr->daddr = raw_addr.sin_addr.s_addr;


	printf("Listening for UDP messages on port %d...\n", udp_port);
	printf("Destination address is %s^%d\n", ip_addr, ip_proto);

	while(1) {
		recv_len = recvfrom(udp_sock, msg + total_received, BUFFER_SIZE - total_received, 0, NULL, NULL);
		if(recv_len < 0) {
			perror("UDP: recvfrom: ");
			continue;
		}

		total_received += recv_len;

		if(total_received >= BUFFER_SIZE) {
			memcpy(buffer + sizeof(struct iphdr), msg, recv_len);
			sent_len = sendto(raw_sock, buffer, sizeof(struct iphdr) + recv_len, 0, (struct sockaddr*) &raw_addr, sizeof(raw_addr));
			if(sent_len < 0) {
				perror("IP: sendto: ");
			} 
			total_received = 0;
		}
		
	}

	close(udp_sock);
	close(raw_sock);
	return 0;
}

