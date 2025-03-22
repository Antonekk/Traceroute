#ifndef HELPERS_H
#define HELPERS_H

#include <sys/types.h>
#include <stdbool.h>

#define EPRINT(...) fprintf (stderr, __VA_ARGS__)

void iprintf(int i);
void eprintf(const char* str);
u_int16_t compute_icmp_checksum(const void *buff, int length);
bool checkValidIP(char *ip);



#endif