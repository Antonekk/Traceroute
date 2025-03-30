#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "handlers.h"
#include "helpers.h"





int main(int argc, char* argv[]){

    // Check if arg number is appropriate
    if (argc != 2){
        EPRINT("Invalid argument count\n");
        exit(EXIT_FAILURE);
    }

    // Check if given ip is valid
    if(!checkValidIP(argv[1])){
        EPRINT("Invalid ip adress\n");
        exit(EXIT_FAILURE);
    }

    // Try to open new socket, if this operation fails - quit
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socket_fd < 0)
        eprintf("Socket: ");
    

    // Pid for id in ICPM header
    pid_t pid = getpid();
    iprintf(pid);

    for(int ttl = 1; ttl <= 30; ttl++){
        
        // Change current TTL to the one given by ttl
        if (setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) != 0)
            eprintf("Change TTL: ");
        

        send_packets(socket_fd,argv[1],pid, ttl);
        recive_packets(socket_fd, pid, ttl);

    }
    


    


}