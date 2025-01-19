#include <arpa/inet.h>
#include <stdio.h>

char *inet_ntoa(struct in_addr in) {
    static char buf[INET_ADDRSTRLEN];
    unsigned char *bytes = (unsigned char *)&in;
    sprintf(buf, "%u.%u.%u.%u", 
            (unsigned int)bytes[0], 
            (unsigned int)bytes[1],
            (unsigned int)bytes[2], 
            (unsigned int)bytes[3]);
    return buf;
}