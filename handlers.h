// Antoni Strasz 339096

#ifndef HANDLERS_H
#define HANDLERS_H
#include <stdbool.h>


#define WAITTIME 1 
#define MS_TO_U 1000
#define SEC_TO_U 1000000
// Handle sending packets
void send_packets(int socket_fd, char *ip, pid_t id, int seq);
bool recive_packets(int socket_fd, pid_t pid, int ttl, char* destination);
#endif