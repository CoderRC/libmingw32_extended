#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res) {
    struct addrinfo *ai;
    struct sockaddr_in *sa;
    unsigned short port = 0;

    if (!node && !service) {
        return EAI_NONAME;
    }

    if (hints) {
        if (hints->ai_family != AF_INET && hints->ai_family != AF_UNSPEC) {
            return EAI_FAMILY;
        }
        if (hints->ai_socktype != 0 && 
            hints->ai_socktype != SOCK_STREAM && 
            hints->ai_socktype != SOCK_DGRAM) {
            return EAI_SOCKTYPE;
        }
    }

    ai = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    if (!ai) {
        return EAI_MEMORY;
    }
    
    // addr family, socket type and protocol
    ai->ai_family = (hints && hints->ai_family) ? hints->ai_family : AF_INET;
    ai->ai_socktype = (hints && hints->ai_socktype) ? hints->ai_socktype : SOCK_STREAM;
    ai->ai_protocol = (hints && hints->ai_protocol) ? hints->ai_protocol : 0;
    
    // Allocate and setup sockaddr
    ai->ai_addr = calloc(1, sizeof(struct sockaddr_in));
    if (!ai->ai_addr) {
        free(ai);
        return EAI_MEMORY;
    }
    
    sa = (struct sockaddr_in *)ai->ai_addr;
    sa->sin_family = AF_INET;
    
    // Handle service (port)
    if (service) {
        char *endptr;
        port = (unsigned short)strtol(service, &endptr, 10);
        if (*endptr != '\0') {// Service is not a number, return error as we don't support service names
            free(ai->ai_addr);
            free(ai);
            return EAI_SERVICE;
        }
        sa->sin_port = htons(port);
    }
    
    // Handle node (address)
    if (node) {
        if (hints && (hints->ai_flags & AI_NUMERICHOST)) {
            if (inet_pton(AF_INET, node, &sa->sin_addr) != 1) {
                free(ai->ai_addr);
                free(ai);
                return EAI_NONAME;
            }
        } else {
            if (inet_pton(AF_INET, node, &sa->sin_addr) != 1) {
                // If numeric conversion fails and AI_NUMERICHOST not set,
                // return error as we don't support DNS resolution
                free(ai->ai_addr);
                free(ai);
                return EAI_NONAME;
            }
        }
    } else {use INADDR_ANY if AI_PASSIVE is set
        if (hints && (hints->ai_flags & AI_PASSIVE)) {
            sa->sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            sa->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        }
    }
    
    ai->ai_addrlen = sizeof(struct sockaddr_in);
    ai->ai_canonname = NULL;
    ai->ai_next = NULL;
    
    *res = ai;
    return 0;
}

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

const char *gai_strerror(int errcode) {
    switch (errcode) {
        case EAI_AGAIN:
            return "Temporary failure in name resolution";
        case EAI_BADFLAGS:
            return "Invalid value for ai_flags";
        case EAI_FAIL:
            return "Non-recoverable failure in name resolution";
        case EAI_FAMILY:
            return "ai_family not supported";
        case EAI_MEMORY:
            return "Memory allocation failure";
        case EAI_NONAME:
            return "Name or service not known";
        case EAI_SERVICE:
            return "Service name not supported for ai_socktype";
        case EAI_SOCKTYPE:
            return "ai_socktype not supported";
        case EAI_SYSTEM:
            return "System error returned in errno";
        case EAI_OVERFLOW:
            return "Argument buffer overflow";
        default:
            return "Unknown error";
    }
}