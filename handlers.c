// Antoni Strasz 339096

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
#include <sys/select.h>



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
    header.un.echo.id = htons(pid);
    for(int i = 0; i < 3; i++){
        // Create unique sequence number to identify packet
        header.un.echo.sequence = htons(3*ttl + i);

        // ahhh, i spent like an hour on this (checksum needs to be set to 0 before every call to checksum)
        header.checksum = 0;
        header.checksum = compute_icmp_checksum((u_int16_t*)&header, sizeof(header));

        // Send constructed package
        ssize_t sbytes = sendto(socket_fd, &header, sizeof(header), 0, (struct sockaddr*)&recipent, sizeof(recipent));
        
        #ifdef DEBUG
            printf("(%d) Sending Packet to %s\n", ntohs(header.un.echo.sequence), ip);        
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
        struct iphdr* ip_echo = (struct iphdr*)((u_int8_t*)icmp_header + sizeof(struct icmphdr));
        icmp_header = (struct icmphdr*)((u_int8_t*)ip_echo + 4 * ip_echo->ihl);
    }

    // Validate id and seq number (convert from network byte order)
    if (ntohs(icmp_header->un.echo.id) == pid &&
        ntohs(icmp_header->un.echo.sequence) >= 3 * ttl &&
        ntohs(icmp_header->un.echo.sequence) < 3 * ttl + 3) {
        return true;
    }

    #ifdef DEBUG
    printf("INVALID_PACKET: received ID %d | expected ID %d\n",
           ntohs(icmp_header->un.echo.id), pid);
    #endif
    return false;
    
}

bool handle_packet(int socket_fd, pid_t pid, int ttl, char ips[3][20], int8_t idx){

    // Init recv structure
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];

    // Recv data from socket
    ssize_t packet_len = recvfrom(socket_fd, buffer, IP_MAXPACKET, 0, (struct sockaddr*) &sender, &sender_len);
    if (packet_len < 0){
        eprintf("recvfrom: ");
    }

    // Retrive headers
    struct iphdr* ip_header = (struct iphdr*) buffer;
    u_int8_t *icmp_packet = buffer + 4 * (ip_header->ihl);
    struct icmphdr* icmp_header = (struct icmphdr*)icmp_packet;

    // Check if packet is valid
    if(!validate_packet(icmp_header, pid, ttl)){
       return false;
    }

    // Get str form of addr
    if(!inet_ntop(AF_INET, &(sender.sin_addr), ips[idx], sizeof(ips[idx]))){
        eprintf("inet_ntop: ");
    }

    return true;

}


bool display_result(char ips[3][20], int times[3], int idx, int ttl, char* destination){

    int delay = 0;
    bool end_reached = false;


    // Alinment doesnt break for ttl's >= 10 (at first)
    char* alinment= " ";
    if(ttl >= 10){
        alinment = "";
    }

    printf("%d.%s ", ttl, alinment);

    // No responces
    if(idx == 0){
        printf("*\n");
        return false;
    }

    for(int i = 0; i < idx; i++){

        // Check for repeats
        bool unique = true;
        for(int j = i - 1; j >= 0; j--){
            if(strcmp(ips[i], ips[j]) == 0){
                unique = false;
                break;
            }
        }
        if(unique){
            printf("%s  ", ips[i]);
        }

        // Check for destination ip addr
        if(strcmp(ips[i], destination) == 0){
            end_reached = true;
        }

        // Sumdelays
        delay+= times[i];
    }

    // Not all packages returned
    if(idx != 3){
        printf("?\n");
    }
    else{ 
        // Print avg 
        printf("%.3f ms\n", (float)delay/(MS_TO_U*idx));
    }

    return end_reached;
}

bool recive_packets(int socket_fd, pid_t pid, int ttl, char* destination){

    fd_set read_fds;
    
    // For time calc (from select's man)
    struct timeval timeout;

    timeout.tv_sec = WAITTIME;
    timeout.tv_usec = 0;


    char ips[3][20];
    int times[3];

    int8_t idx = 0;
    while(idx < 3){

        // TODO: It should be ppb fine to set it once
        FD_ZERO(&read_fds);
        FD_SET(socket_fd, &read_fds);
    
        int ready = select(socket_fd+1, &read_fds, NULL, NULL, &timeout);
        if (ready < 0){
            eprintf("select: ");
        } 
        else if( ready == 0){
            break;
        }
        if(handle_packet(socket_fd,pid,ttl, ips, idx)){
            // Save time and move index
            times[idx] = (WAITTIME * SEC_TO_U) - ((timeout.tv_sec * (uint64_t)SEC_TO_U) + timeout.tv_usec);
            idx++;
        }

    }

    return display_result(ips, times, idx, ttl, destination);
}