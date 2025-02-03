#ifndef _NETDB_H
#define _NETDB_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EAI_AGAIN       11002  // WSATRY_AGAIN
#define EAI_BADFLAGS    10022  // WSAEINVAL
#define EAI_FAIL        11003  // WSANO_RECOVERY
#define EAI_FAMILY      10047  // WSAEAFNOSUPPORT
#define EAI_MEMORY      10012  // WSA_NOT_ENOUGH_MEMORY
#define EAI_NONAME      11001  // WSAHOST_NOT_FOUND
#define EAI_SERVICE     10109  // WSATYPE_NOT_FOUND
#define EAI_SOCKTYPE    10044  // WSAESOCKTNOSUPPORT
#define EAI_SYSTEM      10006  // Handled via errno
#define EAI_OVERFLOW    10014  // WSAEFAULT

struct addrinfo {
    int             ai_flags;      // Input flags
    int             ai_family;     // AF_xxx
    int             ai_socktype;   // SOCK_xxx
    int             ai_protocol;   // 0 or IPPROTO_xxx
    size_t          ai_addrlen;    // Length of ai_addr
    char            *ai_canonname; // Canonical name
    struct sockaddr *ai_addr;      // Binary address
    struct addrinfo *ai_next;      // Next structure
};

#define AI_PASSIVE          0x0001  // Socket for bind()
#define AI_CANONNAME        0x0002  // Return canonical name
#define AI_NUMERICHOST      0x0004  // Disable name resolution
#define AI_NUMERICSERV      0x0008  // Disable service resolution
#define AI_ALL              0x0100  // Query both IPv4 and IPv6
#define AI_ADDRCONFIG       0x0400  // Use configured addresses
#define AI_V4MAPPED         0x0800  // Map IPv4 to IPv6

#define NI_NOFQDN       0x01
#define NI_NUMERICHOST  0x02
#define NI_NAMEREQD     0x04
#define NI_NUMERICSERV  0x08
#define NI_DGRAM        0x10
#define NI_MAXHOST      1025
#define NI_MAXSERV      32

#define IPPORT_RESERVED 1024

struct hostent {
    char  *h_name;      // Official host name
    char  **h_aliases;  // Alias list
    short h_addrtype;   // Address type (AF_INET/AF_INET6)
    short h_length;     // Address length
    char  **h_addr_list; // Address list
};
#define h_addr h_addr_list[0]

struct netent {
    char     *n_name;    // Official network name
    char     **n_aliases; // Alias list
    short    n_addrtype; // Address type (AF_INET)
    uint32_t n_net;      // Network number (host byte order)
};

struct protoent {
    char  *p_name;    // Official protocol name
    char  **p_aliases; // Alias list
    int    p_proto;   // Protocol number
};

struct servent {
    char  *s_name;    // Official service name
    char  **s_aliases; // Alias list
    short s_port;     // Port number (network byte order)
    char  *s_proto;   // Protocol name
};

void             freeaddrinfo(struct addrinfo*);
int              getaddrinfo(const char*, const char*, const struct addrinfo*, struct addrinfo**);
int              getnameinfo(const struct sockaddr*, socklen_t, char*, size_t, char*, size_t, int);
const char      *gai_strerror(int);

struct hostent  *gethostbyaddr(const char*, int, int);
struct hostent  *gethostbyname(const char*);
struct servent  *getservbyport(int, const char*);

#ifdef __cplusplus
}
#endif

#endif /* _NETDB_H */