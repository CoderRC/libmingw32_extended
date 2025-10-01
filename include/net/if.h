#ifndef _NET_IF_H
#define _NET_IF_H

#include <sys/types.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IF_NAMESIZE 16
#define IFNAMSIZ    IF_NAMESIZE

/* Interface flags */
#define IFF_UP          0x1     /* Interface is up */
#define IFF_BROADCAST   0x2     /* Broadcast address valid */
#define IFF_DEBUG       0x4     /* Turn on debugging */
#define IFF_LOOPBACK    0x8     /* Is a loopback net */
#define IFF_POINTOPOINT 0x10    /* Interface is point-to-point link */
#define IFF_NOTRAILERS  0x20    /* Avoid use of trailers */
#define IFF_RUNNING     0x40    /* Resources allocated */
#define IFF_NOARP       0x80    /* No address resolution protocol */
#define IFF_PROMISC     0x100   /* Receive all packets */
#define IFF_ALLMULTI    0x200   /* Receive all multicast packets */
#define IFF_MULTICAST   0x1000  /* Supports multicast */

struct if_nameindex {
    unsigned  if_index;  /* Numeric index of the interface */
    char     *if_name;   /* Null-terminated name of the interface */
};

struct ifreq {
    char ifr_name[IFNAMSIZ];    /* Interface name */
    union {
        struct sockaddr ifru_addr;
        struct sockaddr ifru_dstaddr;
        struct sockaddr ifru_broadaddr;
        struct sockaddr ifru_netmask;
        struct sockaddr ifru_hwaddr;
        short           ifru_flags;
        int             ifru_ivalue;
        int             ifru_mtu;
        char            ifru_data[1];
    } ifr_ifru;
};

#define ifr_addr        ifr_ifru.ifru_addr
#define ifr_dstaddr     ifr_ifru.ifru_dstaddr
#define ifr_broadaddr   ifr_ifru.ifru_broadaddr
#define ifr_netmask     ifr_ifru.ifru_netmask
#define ifr_flags       ifr_ifru.ifru_flags
#define ifr_hwaddr      ifr_ifru.ifru_hwaddr
#define ifr_metric      ifr_ifru.ifru_ivalue
#define ifr_mtu         ifr_ifru.ifru_mtu
#define ifr_data        ifr_ifru.ifru_data

void                  if_freenameindex(struct if_nameindex *);
char                 *if_indextoname(unsigned, char *);
struct if_nameindex  *if_nameindex(void);
unsigned              if_nametoindex(const char *);

#ifdef __cplusplus
}
#endif

#endif /* _NET_IF_H */