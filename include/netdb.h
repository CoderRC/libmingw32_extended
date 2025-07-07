/* include/netdb.h */

#ifndef _NETDB_H
#define _NETDB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <netinet/in.h>

/* Error codes for getaddrinfo */
#define EAI_SUCCESS     0   /* Success */
#define EAI_FAMILY      1   /* ai_family not supported */
#define EAI_AGAIN       2   /* Temporary failure in name resolution */
#define EAI_NONAME      3   /* Name or service not known */
#define EAI_FAIL        4   /* Non-recoverable failure in name resolution */
#define EAI_BADFLAGS    5   /* Invalid value for ai_flags */
#define EAI_MEMORY      6   /* Memory allocation failure */
#define EAI_SERVICE     8   /* Servname not supported for ai_socktype */
#define EAI_SOCKTYPE    10  /* ai_socktype not supported */

/* Address info flags */
#define AI_PASSIVE      0x00000001  /* Socket address is intended for bind */
#define AI_CANONNAME    0x00000002  /* Request canonical name */
#define AI_NUMERICHOST  0x00000004  /* Nodename must be numeric */
#define AI_NUMERICSERV  0x00000008  /* Servname must be numeric */
#define AI_V4MAPPED     0x00000800  /* Map IPv4 addresses to IPv6 */
#define AI_ALL          0x00000100  /* Return all addresses (IPv4 and IPv6) */
#define AI_ADDRCONFIG   0x00000400  /* Use addresses matching local configuration */

/* Structure for address information */
struct addrinfo {
    int ai_flags;               /* Input flags (AI_*) */
    int ai_family;              /* Address family (AF_INET, AF_INET6, etc.) */
    int ai_socktype;            /* Socket type (SOCK_STREAM, SOCK_DGRAM, etc.) */
    int ai_protocol;            /* Protocol (IPPROTO_TCP, IPPROTO_UDP, etc.) */
    socklen_t ai_addrlen;       /* Length of ai_addr */
    struct sockaddr *ai_addr;   /* Socket address */
    char *ai_canonname;         /* Canonical name for host */
    struct addrinfo *ai_next;   /* Next structure in linked list */
};

/* Function prototypes */
int getaddrinfo(const char *restrict nodename,
                const char *restrict servname,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res);
void freeaddrinfo(struct addrinfo *res);
const char *gai_strerror(int errcode);

#ifdef __cplusplus
}
#endif

#endif /* _NETDB_H */
