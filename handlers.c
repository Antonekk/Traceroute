#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>


#include <netinet/ip_icmp.h>
#include <netinet/ip.h>


#include "handlers.h"
#include "helpers.h"



/*
=============================================
SEND
=============================================
*/


// Handle packet sending
void send_packets(int socket_fd, char *ip, pid_t pid, int ttl){

    // Init new recipent for packets (set all bits to 0)
    // TODO, potentialy could mbe done outside this func 
    struct sockaddr_in recipent;
    memset(&recipent, 0, sizeof(recipent));
    
    // Set recipent family to IPv4
    recipent.sin_family = AF_INET;
    
    // Generate sin.addr based on text representation ip
    int res = inet_pton(AF_INET, ip,&recipent.sin_addr);
    if (res <= 0){
        // recipent does not contain a valid network addr in specified family
        if(res == 0){
            EPRINT("Not in presentation format");
            exit(EXIT_FAILURE);
        }
        else{
            eprintf("inet_pton: ");
        }
    }


    // Set basic fields for header structure
    struct icmphdr header;
    header.type = ICMP_ECHO;
    header.code = 0;
    header.checksum = 0;
    header.un.echo.id = htons(pid);
    for(int i = 0; i < 3; i++){
        // Create unique sequence number to identify packet
        header.un.echo.sequence = htons(3*ttl + i);

        header.checksum = compute_icmp_checksum((u_int16_t*)&header, sizeof(header));

        ssize_t sbytes = sendto(socket_fd, &header, sizeof(header), 0, (struct sockaddr*)&recipent, sizeof(recipent));
        
        #ifdef DEBUG
            printf("(%d) Sending Packet to %s\n", header.un.echo.sequence, ip);        
        #endif

        if (sbytes < 0){
            eprintf("sendto: ");
        }
    }
}



/*
=============================================
RECIVE
=============================================
*/


// Check if given package is valid 
bool validate_packet(struct icmphdr* icmp_header, pid_t pid, int ttl){

    // Valid package is either a direct reply, or time was exceeded
    if(icmp_header->type != ICMP_ECHOREPLY && icmp_header->type != ICMP_TIME_EXCEEDED){
        return false;
    }

    // In case of time exceeded, get the original ICMP Echo Request header
    if (icmp_header->type == ICMP_TIME_EXCEEDED) {
        struct iphdr* ip_echo = (struct iphdr*)((void*)icmp_header + sizeof(struct icmphdr));
        icmp_header = (struct icmphdr*)((void*)ip_echo + 4 * ip_echo->ihl);
    }

    // Validate id and seq number (convert from network byte order)
    if (ntohs(icmp_header->un.echo.id) == pid &&
        ntohs(icmp_header->un.echo.sequence) >= 3 * ttl &&
        ntohs(icmp_header->un.echo.sequence) < 3 * ttl + 3) {
        return true;
    }

    printf("INVALID_PACKET: received ID %d | expected ID %d\n",
           ntohs(icmp_header->un.echo.id), pid);
    return false;
    
}

bool handle_packet(int socket_fd, pid_t pid, int ttl){
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];

    ssize_t packet_len = recvfrom(socket_fd, buffer, IP_MAXPACKET, 0, (struct sockaddr*) &sender, &sender_len);
    if (packet_len < 0){
        eprintf("recvfrom: ");
    }


    char ip_addr[20];
    if(!inet_ntop(AF_INET, &(sender.sin_addr), ip_addr, sizeof(ip_addr))){
        eprintf("inet_ntop: ");
    }

    struct iphdr* ip_header = (struct iphdr*) buffer;
    u_int8_t *icmp_packet = buffer + 4 * (ip_header->ihl);
    struct icmphdr* icmp_header = (struct icmphdr*)icmp_packet;

    printf("(%d) IP: %s\n", ttl, ip_addr);
    return validate_packet(icmp_header, pid, ttl);
}


void recive_packets(int socket_fd, pid_t pid, int ttl){

    struct pollfd pfd;
    pfd.fd = socket_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    int8_t packets_left = 3;
    while(packets_left > 0){
        int ready = poll(&pfd, 1, WAITTIME);
        if (ready < 0){
            eprintf("select: ");
        } 
        else if( ready == 0){
            break;
        }
        if(handle_packet(socket_fd,pid,ttl)){
            packets_left--;
        }



    }

    return;
}