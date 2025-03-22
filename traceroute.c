#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

#include "handlers.h"
#include "helpers.h"


int main(int argc, char* argv[]){

    if (argc != 2){
        EPRINT("Invalid argument count\n");
        exit(EXIT_FAILURE);
    }

    char *ip_addr;
    if(!checkValidIP(argv[1])){
        ip_addr = argv[1];
        EPRINT("Invalid ip adress\n");
        exit(EXIT_FAILURE);
    }

    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socket_fd < 0)
        eprintf("Socket: ");
    
    send_packets(socket_fd,ip_addr );
    


    


}