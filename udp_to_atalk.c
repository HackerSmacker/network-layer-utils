#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <netatalk/at.h>
#include <unistd.h>

#define BUFFER_SIZE 1316
#define DDP_MAX_DATA_SIZE 586

int main(int argc, char** argv) {
    int udp_sock, ddp_sock, udp_port;
    unsigned char atalk_addr, atalk_port;
    unsigned short atalk_net;
    struct sockaddr_in udp_addr;
    char udp_buffer[BUFFER_SIZE];
    struct sockaddr_at ddp_addr;
    char buffer[BUFFER_SIZE];
    int total_received = 0;
    ssize_t recv_len, sent_len;
    
    if (argc != 5) {
        printf("Usage: %s <UDP port> <destination network> <destination address> <destination port>\n", argv[0]);
        printf("Using 0 for the destination network is equivalent to specifying the current network.\n");
        printf("Port numbers below 129 are reserved, do not use them.\n");
        exit(1);
    }
    udp_port = atoi(argv[1]);
    atalk_net = atoi(argv[2]);
    atalk_addr = atoi(argv[3]);
    atalk_port = atoi(argv[4]);

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

    ddp_sock = socket(AF_APPLETALK, SOCK_DGRAM, 0);
    if (ddp_sock < 0) {
        perror("DDP: socket: ");
        close(udp_sock);
        exit(-1);
    }

    memset(&ddp_addr, 0, sizeof(ddp_addr));
    ddp_addr.sat_family = AF_APPLETALK;
    ddp_addr.sat_port = atalk_port;
    ddp_addr.sat_addr.s_net = atalk_net; /* ATADDR_ANYNET; */
    ddp_addr.sat_addr.s_node = atalk_addr; /* ATADDR_BCAST; */

    printf("Listening for UDP messages on port %d...\n", udp_port);
    printf("Destination address is %d.%d:%d\n", atalk_net, atalk_addr, atalk_port);

    while (1) {
        recv_len = recvfrom(udp_sock, buffer + total_received, BUFFER_SIZE - total_received, 0, NULL, NULL);
        if (recv_len < 0) {
            perror("UDP: recvfrom: ");
            continue;
        }

        total_received += recv_len;

        // Process data in chunks of DDP_MAX_DATA_SIZE
        while (total_received > 0) {
            // Determine how much data to send: max DDP_MAX_DATA_SIZE - 1 (for the port byte)
            int send_size = (total_received > DDP_MAX_DATA_SIZE - 1) ? DDP_MAX_DATA_SIZE - 1 : total_received;

            // Create the packet with the first byte as the sat_port value
            unsigned char packet[DDP_MAX_DATA_SIZE];
            packet[0] = 0xff; // Set the first byte to 0xFF
            memcpy(packet + 1, buffer, send_size); // Copy data after the first byte

            // Send the packet
            sent_len = sendto(ddp_sock, packet, send_size + 1, 0, (struct sockaddr*) &ddp_addr, sizeof(ddp_addr));
            if (sent_len < 0) {
                perror("DDP: sendto: ");
            }

            // Shift remaining data
            total_received -= send_size;
            memmove(buffer, buffer + send_size, total_received); // Move the remaining data to the front

            // If we sent the last of the received data, break
            if (total_received == 0) {
                break;
            }
        }
    }

    close(udp_sock);
    close(ddp_sock);
    return 0;
}

