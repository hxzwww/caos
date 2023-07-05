#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <time.h>

#define PACKET_SIZE 32

unsigned short checksum(unsigned short *buf, int len) {
    unsigned long sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(unsigned char*)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main(int argc, char **argv) {
    char *ip_address = argv[1];
    int timeout = atoi(argv[2]);
    int interval = atoi(argv[3]);

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_aton(ip_address, &addr.sin_addr);

    int num_packets_sent = 0;
    int num_packets_received = 0;
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    while (1) {
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        if ((current_time.tv_sec - start_time.tv_sec) >= timeout) {
            break;
        }

        char packet[PACKET_SIZE];
        memset(packet, 0, sizeof(packet))
        struct icmphdr *icmp_hdr_sent = (struct icmphdr*)packet;
        icmp_hdr_sent->type = ICMP_ECHO;
        icmp_hdr_sent->code = 0;
        icmp_hdr_sent->un.echo.id = getpid() & 0xFFFF;
        icmp_hdr_sent->un.echo.sequence = num_packets_sent & 0xFFFF;
        icmp_hdr_sent->checksum = checksum((unsigned short*)icmp_hdr_sent, sizeof(struct icmphdr));

        int bytes_sent = sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr*)&addr, sizeof(addr));

        ++num_packets_sent;

        char recv_packet[PACKET_SIZE];
        memset(recv_packet, 0, sizeof(recv_packet));
        struct sockaddr_in recv_addr;
        socklen_t recv_addr_len = sizeof(recv_addr);

        int bytes_received = recvfrom(sockfd,
                                      recv_packet,
                                      sizeof(recv_packet),
                                      MSG_DONTWAIT,
                                      (struct sockaddr*)&recv_addr,
                                      &recv_addr_len);
        if (bytes_received > 0) {
            struct iphdr *ip_hdr_recieved = (struct iphdr*)recv_packet;
            struct icmphdr *icmp_hdr_recieved = (struct icmphdr*)(recv_packet + (ip_hdr_recieved->ihl * 4));
            if ((icmp_hdr_recieved->type == ICMP_ECHOREPLY) && (icmp_hdr_recieved->un.echo.id == (getpid() & 0xFFFF))) {
                // if (icmp_hdr_sent->checksum == checksum((unsigned short*)icmp_hdr_recieved, sizeof(struct icmphdr))) {
                    ++num_packets_received;
                // }
            }
        }

        usleep(interval);
    }

    printf("%d", num_packets_received);
    close(sockfd);

    return 0;
}
