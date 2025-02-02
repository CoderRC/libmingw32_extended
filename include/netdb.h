#ifndef _NETDB_H
#define _NETDB_H

#include <sys/socket.h>
#include <netinet/in.h>

// Error codes for getaddrinfo
#define EAI_AGAIN      -3  /* Temporary failure in name resolution */
#define EAI_BADFLAGS   -1  /* Invalid value for ai_flags */
#define EAI_FAIL       -4  /* Non-recoverable failure in name resolution */
#define EAI_FAMILY     -6  /* ai_family not supported */
#define EAI_MEMORY     -10 /* Memory allocation failure */
#define EAI_NONAME     -2  /* Name or service not known */
#define EAI_SERVICE    -8  /* Service name not supported for ai_socktype */
#define EAI_SOCKTYPE   -7  /* ai_socktype not supported */
#define EAI_SYSTEM     -11 /* System error returned in errno */
#define EAI_OVERFLOW   -12 /* Argument buffer overflow */

// Flags for addrinfo
#define AI_PASSIVE     0x0001 /* Socket address is intended for bind() */
#define AI_CANONNAME   0x0002 /* Request canonical name */
#define AI_NUMERICHOST 0x0004 /* Don't use name resolution */
#define AI_NUMERICSERV 0x0008 /* Don't use service name resolution */
#define AI_V4MAPPED    0x0010 /* IPv4 mapped addresses if no IPv6 */
#define AI_ALL         0x0020 /* Return both IPv4 and IPv6 addresses */
#define AI_ADDRCONFIG  0x0040 /* Return addresses for each configured protocol */

struct addrinfo {
    int               ai_flags;      /* Input flags */
    int               ai_family;     /* Address family (AF_xxx) */
    int               ai_socktype;   /* Socket type (SOCK_xxx) */
    int               ai_protocol;   /* Protocol (IPPROTO_xxx) */
    socklen_t         ai_addrlen;    /* Length of socket address */
    struct sockaddr  *ai_addr;       /* Socket address */
    char             *ai_canonname;  /* Canonical name of host */
    struct addrinfo  *ai_next;       /* Next address in linked list */
};

// Function prototypes
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

void freeaddrinfo(struct addrinfo *res);

const char *gai_strerror(int errcode);

#endif /* _NETDB_H */