#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/ip_icmp.h>

#include "handlers.h"
#include "helpers.h"


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
    header.un.echo.id = pid;
    for(int i = 0; i < 3; i++){
        // Create unique sequence number to identify packet
        header.un.echo.sequence = 3*ttl + i;

        header.checksum = compute_icmp_checksum((u_int16_t*)&header, sizeof(header));

        ssize_t sbytes = sendto(socket_fd, &header, sizeof(header), 0, (struct sockaddr*)&recipent, sizeof(recipent));

        if (sbytes < 0){
            eprintf("sendto: ");
        }
    }
}


void recive_packets(int socket_fd, pid_t pid, int ttl){
    
}