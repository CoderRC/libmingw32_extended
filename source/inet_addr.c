#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>

in_addr_t inet_addr(const char *cp) {
    unsigned int a, b, c, d;
    uint32_t addr = 0;
    
    if (sscanf(cp, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
        return INADDR_NONE;
        
    if (a > 255 || b > 255 || c > 255 || d > 255)
        return INADDR_NONE;
        
    addr = (a << 24) | (b << 16) | (c << 8) | d;
    return htonl(addr);
}