/* include/netdb.h */

#ifndef _NETDB_H
#define _NETDB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>

/* Reserved port number */
#define IPPORT_RESERVED 1024

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
#define EAI_SYSTEM      11  /* System error */
#define EAI_OVERFLOW    12  /* Argument buffer overflow */

/* Address info flags */
#define AI_PASSIVE      0x00000001  /* Socket address is intended for bind */
#define AI_CANONNAME    0x00000002  /* Request canonical name */
#define AI_NUMERICHOST  0x00000004  /* Nodename must be numeric */
#define AI_NUMERICSERV  0x00000008  /* Servname must be numeric */
#define AI_V4MAPPED     0x00000800  /* Map IPv4 addresses to IPv6 */
#define AI_ALL          0x00000100  /* Return all addresses (IPv4 and IPv6) */
#define AI_ADDRCONFIG   0x00000400  /* Use addresses matching local configuration */

/* getnameinfo flags */
#define NI_NOFQDN       0x00000001  /* Only nodename portion for local hosts */
#define NI_NUMERICHOST  0x00000002  /* Numeric form of hostname */
#define NI_NAMEREQD     0x00000004  /* Error if hostname not found */
#define NI_NUMERICSERV  0x00000008  /* Numeric form of service */
#define NI_NUMERICSCOPE 0x00000020  /* Numeric form of scope */
#define NI_DGRAM        0x00000010  /* Service is datagram */

/* Host entry structure */
struct hostent {
    char *h_name;       /* Official name of host */
    char **h_aliases;   /* Alias list */
    int h_addrtype;     /* Address type */
    int h_length;       /* Length of address */
    char **h_addr_list; /* List of addresses */
};

/* Network entry structure */
struct netent {
    char *n_name;       /* Official name of network */
    char **n_aliases;   /* Alias list */
    int n_addrtype;     /* Net address type */
    uint32_t n_net;     /* Network number */
};

/* Protocol entry structure */
struct protoent {
    char *p_name;       /* Official protocol name */
    char **p_aliases;   /* Alias list */
    int p_proto;        /* Protocol number */
};

/* Service entry structure */
struct servent {
    char *s_name;       /* Official service name */
    char **s_aliases;   /* Alias list */
    int s_port;         /* Port number */
    char *s_proto;      /* Protocol to use */
};

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
int getnameinfo(const struct sockaddr *restrict addr, socklen_t addrlen,
                char *restrict host, socklen_t hostlen,
                char *restrict serv, socklen_t servlen, int flags);

/* Host database functions */
void endhostent(void);
struct hostent *gethostent(void);
void sethostent(int stayopen);

/* Network database functions */
void endnetent(void);
struct netent *getnetbyaddr(uint32_t net, int type);
struct netent *getnetbyname(const char *name);
struct netent *getnetent(void);
void setnetent(int stayopen);

/* Protocol database functions */
void endprotoent(void);
struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynumber(int proto);
struct protoent *getprotoent(void);
void setprotoent(int stayopen);

/* Service database functions */
void endservent(void);
struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);
struct servent *getservent(void);
void setservent(int stayopen);

#ifdef __cplusplus
}
#endif

#endif /* _NETDB_H */
