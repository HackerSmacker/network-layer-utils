#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netipx/ipx.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define UDP_PORT 8080
#define IPX_PORT 0x5000
#define BUFFER_SIZE 1316

int main(int argc, char** argv) {
    struct sockaddr_ipx ipx_addr;
    struct sockaddr_in udp_addr;
    int ipx_sock, udp_sock;
    char msg[BUFFER_SIZE];
    int len, rc;
    ssize_t sent_len;
    const char* udp_ip = argv[1];
    int udp_port = atoi(argv[2]);
    char buffer[BUFFER_SIZE];
    int total_received = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <UDP_IP> <UDP_PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    ipx_sock = socket(AF_IPX, SOCK_DGRAM, 0);
    if (ipx_sock < 0) {
        perror("IPX: socket: ");
        exit(EXIT_FAILURE);
    }

    memset(&ipx_addr, 0, sizeof(ipx_addr));
    ipx_addr.sipx_family = AF_IPX;
    ipx_addr.sipx_network = 0;
    ipx_addr.sipx_port = htons(IPX_PORT);
    ipx_addr.sipx_type = 17;

    if (bind(ipx_sock, (struct sockaddr*) &ipx_addr, sizeof(ipx_addr)) < 0) {
        perror("IPX: bind: ");
        close(ipx_sock);
        exit(EXIT_FAILURE);
    }

    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock < 0) {
        perror("UDP: socket: ");
        close(ipx_sock);
        exit(EXIT_FAILURE);
    }

    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_port = htons(udp_port);
    if (inet_pton(AF_INET, udp_ip, &udp_addr.sin_addr) <= 0) {
        perror("UDP: inet_pton: ");
        close(ipx_sock);
        close(udp_sock);
        exit(EXIT_FAILURE);
    }

    printf("Listening for IPX messages on port %04x...\n", IPX_PORT);
    printf("Destination: %s:%d\n", udp_ip, udp_port);


    while (1) {
        len = sizeof(ipx_addr);
        rc = recvfrom(ipx_sock, msg, sizeof(msg), 0, (struct sockaddr*) &ipx_addr, &len);
        if (rc < 0) {
            perror("IPX: recvfrom: ");
            continue;
        }

        if (total_received + rc <= BUFFER_SIZE) {
            memcpy(buffer + total_received, msg, rc);
            total_received += rc;

            if (total_received >= BUFFER_SIZE) {
                sent_len = sendto(udp_sock, buffer, total_received, 0, (struct sockaddr*) &udp_addr, sizeof(udp_addr));
                if (sent_len < 0) {
                    perror("UDP: sendto: ");
                } 

                total_received = 0;
            }
        } else {
            printf("[IPX -> UDP] Buffer overflow: Received %d bytes, current total %d bytes\n", rc, total_received);
            total_received = 0;
        }
    }

    close(ipx_sock);
    close(udp_sock);
    return 0;
}

