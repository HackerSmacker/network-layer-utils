#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#include <unistd.h>

#define BUFFER_SIZE 1316
#define DDP_MAX_DATA_SIZE 586

int main(int argc, char** argv) {
    int udp_sock, dn_sock, udp_port;
    unsigned char dn_obj;
    char* dn_netandnode;
    struct sockaddr_in udp_addr;
    char udp_buffer[BUFFER_SIZE];
    struct sockaddr_dn dn_addr;
    struct dn_naddr* binaddr;
    char buffer[BUFFER_SIZE];
    int total_received = 0;
    ssize_t recv_len, sent_len;
    
    if (argc != 4) {
        printf("Usage: %s <UDP port> <destination net.node> <destination object>\n", argv[0]);
        exit(1);
    }
    udp_port = atoi(argv[1]);
    dn_netandnode = argv[2];
    dn_obj = atoi(argv[3]);

    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock < 0) {
        perror("UDP: socket: ");
        exit(-1);
    }

    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(udp_port);

    if (bind(udp_sock, (struct sockaddr*) &udp_addr, sizeof(udp_addr)) < 0) {
        perror("UDP: bind: ");
        close(udp_sock);
        exit(-1);
    }

    dn_sock = socket(AF_DECnet, SOCK_SEQPACKET, DNPROTO_NSP);
    if (dn_sock < 0) {
        perror("DN: socket: ");
        close(udp_sock);
        exit(-1);
    }

    memset(&dn_addr, 0, sizeof(dn_addr));
    dn_addr.sdn_family = AF_DECnet;
    dn_addr.sdn_flags = 0x00;
    dn_addr.sdn_objnum = dn_obj;
    binaddr = dnet_addr(dn_netandnode);
    memcpy(dn_addr.sdn_add.a_addr, binaddr->a_addr, binaddr->a_len);

    if (connect(dn_sock, (struct sockaddr*) &dn_addr, sizeof(dn_addr)) < 0) {
	 perror("DN: connect: ");
	 exit(-1);
    }

    printf("Listening for UDP messages on port %d...\n", udp_port);
    printf("Destination address is %s:%d\n", dn_netandnode, dn_obj);

    while (1) {
        recv_len = recvfrom(udp_sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recv_len < 0) {
            perror("UDP: recvfrom: ");
            continue;
        }

        sent_len = sendto(dn_sock, buffer, recv_len, 0, (struct sockaddr*) &dn_addr, sizeof(dn_addr));
    }

    close(udp_sock);
    close(dn_sock);
    return 0;
}

