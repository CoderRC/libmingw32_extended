#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
    if (dst == NULL) {
        errno = ENOSPC;
        return NULL;
    }

    switch (af) {
        case AF_INET: {
            if (size < INET_ADDRSTRLEN) {
                errno = ENOSPC;
                return NULL;
            }
            
            const uint8_t *bytes = (const uint8_t *)src;
            snprintf(dst, size, "%u.%u.%u.%u",
                    bytes[0], bytes[1], bytes[2], bytes[3]);
            break;
        }
        
        case AF_INET6: {
            if (size < INET6_ADDRSTRLEN) {
                errno = ENOSPC;
                return NULL;
            }
            
            const uint16_t *words = (const uint16_t *)src;
            snprintf(dst, size,
                    "%x:%x:%x:%x:%x:%x:%x:%x",
                    ntohs(words[0]), ntohs(words[1]),
                    ntohs(words[2]), ntohs(words[3]),
                    ntohs(words[4]), ntohs(words[5]),
                    ntohs(words[6]), ntohs(words[7]));
            break;
        }
        
        default:
            errno = EAFNOSUPPORT;
            return NULL;
    }
    
    return dst;
}