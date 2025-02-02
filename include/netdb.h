#ifndef _NETDB_H
#define _NETDB_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#define AI_PASSIVE     0x0001 /* Socket address is intended for bind() */
#define AI_CANONNAME   0x0002 /* Request canonical name */
#define AI_NUMERICHOST 0x0004 /* Don't use name resolution */
#define AI_NUMERICSERV 0x0008 /* Don't use service name resolution */
#define AI_V4MAPPED    0x0010 /* IPv4 mapped addresses if no IPv6 */
#define AI_ALL         0x0020 /* Return both IPv4 and IPv6 addresses */
#define AI_ADDRCONFIG  0x0040 /* Return addresses for each configured protocol */

#define NI_NOFQDN      0x0001 /* Only return nodename portion for local hosts */
#define NI_NUMERICHOST 0x0002 /* Return numeric form of the host's address */
#define NI_NAMEREQD    0x0004 /* Error if the host's name not found */
#define NI_NUMERICSERV 0x0008 /* Return numeric form of the service */
#define NI_DGRAM       0x0010 /* Service is datagram service */
#define NI_MAXHOST     1025   /* Maximum host name length returned */
#define NI_MAXSERV     32     /* Maximum service name length returned */

#define IPPORT_RESERVED 1024  /* Highest reserved Internet port number */

void             endhostent(void);
void             endnetent(void);
void             endprotoent(void);
void             endservent(void);
void             freeaddrinfo(struct addrinfo *);
const char      *gai_strerror(int);
int              getaddrinfo(const char *restrict, const char *restrict,
                    const struct addrinfo *restrict,
                    struct addrinfo **restrict);
struct hostent  *gethostbyaddr(const void *, socklen_t, int);
struct hostent  *gethostbyname(const char *);
struct hostent  *gethostent(void);
int              getnameinfo(const struct sockaddr *restrict, socklen_t,
                    char *restrict, socklen_t, char *restrict,
                    socklen_t, int);
struct netent   *getnetbyaddr(uint32_t, int);
struct netent   *getnetbyname(const char *);
struct netent   *getnetent(void);
struct protoent *getprotobyname(const char *);
struct protoent *getprotobynumber(int);
struct protoent *getprotoent(void);
struct servent  *getservbyname(const char *, const char *);
struct servent  *getservbyport(int, const char *);
struct servent  *getservent(void);
void             sethostent(int);
void             setnetent(int);
void             setprotoent(int);
void             setservent(int);

struct hostent {
    char  *h_name;            /* Official name of host */
    char **h_aliases;         /* Alias list */
    int    h_addrtype;        /* Host address type */
    int    h_length;          /* Length of address */
    char **h_addr_list;       /* List of addresses */
};
#define h_addr h_addr_list[0] /* Address, for backward compatibility */

struct netent {
    char  *n_name;            /* Official name of net */
    char **n_aliases;         /* Alias list */
    int    n_addrtype;        /* Net address type */
    uint32_t n_net;           /* Network number */
};

struct protoent {
    char  *p_name;            /* Official protocol name */
    char **p_aliases;         /* Alias list */
    int    p_proto;           /* Protocol number */
};

struct servent {
    char  *s_name;            /* Official service name */
    char **s_aliases;         /* Alias list */
    int    s_port;            /* Port number */
    char  *s_proto;           /* Protocol to use */
};

struct addrinfo {
    int              ai_flags;      /* Input flags */
    int              ai_family;     /* Protocol family for socket */
    int              ai_socktype;   /* Socket type */
    int              ai_protocol;   /* Protocol for socket */
    socklen_t        ai_addrlen;    /* Length of socket address */
    struct sockaddr *ai_addr;       /* Socket address for socket */
    char            *ai_canonname;  /* Canonical name for service location */
    struct addrinfo *ai_next;       /* Pointer to next in list */
};

#ifdef __cplusplus
}
#endif

#endif /* _NETDB_H */