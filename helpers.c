#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>


u_int16_t compute_icmp_checksum(const void *buff, int length)
{
    const u_int16_t* ptr = buff;
    u_int32_t sum = 0;
    assert (length % 2 == 0);
    for (; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16U) + (sum & 0xffffU);
    return ~(sum + (sum >> 16U));
}

bool checkValidIP(char *ip)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result != 0;
}