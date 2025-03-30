#ifndef HANDLERS_H
#define HANDLERS_H


#define WAITTIME 1000

// Handle sending packets
void send_packets(int socket_fd, char *ip, pid_t id, int seq);
void recive_packets(int socket_fd, pid_t pid, int ttl);
#endif