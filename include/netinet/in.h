#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <stdint.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Standard Internet Protocol (IP) address types */
typedef uint16_t in_port_t;      // Port number type
typedef uint32_t in_addr_t;      // IPv4 address type

/* Structure for an IPv4 address */
struct in_addr {
    in_addr_t s_addr;            // IPv4 address in network byte order
};

/* Socket address structure for IPv4 */
struct sockaddr_in {
    sa_family_t sin_family;      // Address family (AF_INET for IPv4)
    in_port_t sin_port;          // Port number in network byte order
    struct in_addr sin_addr;     // IPv4 address
    unsigned char sin_zero[8];   // Padding to match struct sockaddr size
};

/* Constants for address families */
#define AF_INET 2               // IPv4 address family

/* Constants for socket types */
#define IPPROTO_TCP 6           // TCP protocol
#define IPPROTO_UDP 17          // UDP protocol

/* Macros for address manipulation */
#define INADDR_ANY ((in_addr_t)0x00000000)      // Bind to any address
#define INADDR_LOOPBACK ((in_addr_t)0x7F000001) // Loopback address (127.0.0.1)
#define INADDR_BROADCAST ((in_addr_t)0xFFFFFFFF) // Broadcast address
#define INADDR_NONE ((in_addr_t)0xFFFFFFFF)     // Invalid address

/* Byte-order conversion functions */
uint16_t htons(uint16_t hostshort);      // Host to network short
uint16_t ntohs(uint16_t netshort);       // Network to host short
uint32_t htonl(uint32_t hostlong);       // Host to network long
uint32_t ntohl(uint32_t netlong);        // Network to host long

/* Address conversion functions */
in_addr_t inet_addr(const char *cp);    // Convert string to IPv4 address
char *inet_ntoa(struct in_addr in);     // Convert IPv4 address to string

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_IN_H */