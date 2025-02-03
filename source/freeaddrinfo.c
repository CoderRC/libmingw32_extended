#include <stdlib.h>
#include <netdb.h>

void freeaddrinfo(struct addrinfo *res) {
    struct addrinfo *next;
    
    while (res) {
        next = res->ai_next;
        free(res->ai_addr);
        free(res->ai_canonname);
        free(res);
        res = next;
    }
}
