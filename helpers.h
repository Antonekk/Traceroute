// Antoni Strasz 339096

#ifndef HELPERS_H
#define HELPERS_H

//#define DEBUG


#include <sys/types.h>
#include <stdbool.h>


// Macro for printion errors
#define EPRINT(...) fprintf (stderr, __VA_ARGS__)

// Wrapper for printing ints
void iprintf(int i);

// Wrapper for error quiting funcion 
void eprintf(const char* str);

// Compute icmp checksum
u_int16_t compute_icmp_checksum(const void *buff, int length);

// Check if given ip adress is a valid one
bool checkValidIP(char *ip);



#endif