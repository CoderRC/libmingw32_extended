#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>  // for size_t, NULL
#include <stdint.h>  // for standard integer types
#include <sys/uio.h>  // for struct iovec

#ifndef _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#define _SSIZE_T_DEFINED
#endif

// Define socklen_t as an unsigned integral type of at least 32 bits
typedef unsigned int socklen_t;

// Define sa_family_t as an unsigned integral type
typedef unsigned short sa_family_t;

// Define sockaddr structure
struct sockaddr {
    sa_family_t sa_family;   // address family
    char sa_data[14];        // socket address (variable-length data)
};

// Define msghdr structure
struct msghdr {
    void *msg_name;             // optional address
    socklen_t msg_namelen;      // size of address
    struct iovec *msg_iov;      // scatter/gather array
    int msg_iovlen;             // members in msg_iov
    void *msg_control;          // ancillary data
    socklen_t msg_controllen;   // ancillary data buffer length
    int msg_flags;              // flags on received message
};

// Define cmsghdr structure
struct cmsghdr {
    socklen_t cmsg_len;     // data byte count, including the cmsghdr
    int cmsg_level;         // originating protocol
    int cmsg_type;          // protocol-specific type
};

// Define linger structure
struct linger {
    int l_onoff;            // indicates whether linger option is enabled
    int l_linger;           // linger time, in seconds
};

// Socket types
#define SOCK_DGRAM       2
#define SOCK_STREAM      1
#define SOCK_SEQPACKET   5

// Level argument for setsockopt() and getsockopt()
#define SOL_SOCKET      0xffff

// Option names for setsockopt() and getsockopt()
#define SO_ACCEPTCONN    0x0002
#define SO_BROADCAST     0x0020
#define SO_DEBUG         0x0001
#define SO_DONTROUTE     0x0010
#define SO_ERROR         0x1007
#define SO_KEEPALIVE     0x0008
#define SO_LINGER        0x0080
#define SO_OOBINLINE     0x0100
#define SO_RCVBUF        0x1002
#define SO_RCVLOWAT      0x1004
#define SO_RCVTIMEO      0x1006
#define SO_REUSEADDR     0x0004
#define SO_SNDBUF        0x1001
#define SO_SNDLOWAT      0x1003
#define SO_SNDTIMEO      0x1005
#define SO_TYPE          0x1008

// Flags for msg_flags in msghdr structure
#define MSG_CTRUNC      0x08
#define MSG_DONTROUTE   0x04
#define MSG_EOR         0x80
#define MSG_OOB         0x01
#define MSG_PEEK        0x02
#define MSG_TRUNC       0x20
#define MSG_WAITALL     0x100

// Address families
#define AF_UNIX         1
#define AF_UNSPEC       0
#define AF_INET         2

// Shutdown options
#define SHUT_RD         0
#define SHUT_WR         1
#define SHUT_RDWR       2

// Function declarations
int accept(int socket, struct sockaddr *address, socklen_t *address_len);
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
int connect(int socket, const struct sockaddr *address, socklen_t address_len);
int getpeername(int socket, struct sockaddr *address, socklen_t *address_len);
int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);
int getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len);
int listen(int socket, int backlog);
ssize_t recv(int socket, void *buffer, size_t length, int flags);
ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
                 struct sockaddr *address, socklen_t *address_len);
ssize_t recvmsg(int socket, struct msghdr *message, int flags);
ssize_t send(int socket, const void *message, size_t length, int flags);
ssize_t sendmsg(int socket, const struct msghdr *message, int flags);
ssize_t sendto(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len);
int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
int shutdown(int socket, int how);
int socket(int domain, int type, int protocol);
int socketpair(int domain, int type, int protocol, int socket_vector[2]);

// Macros for accessing ancillary data
#define CMSG_DATA(cmsg)          ((unsigned char *)((struct cmsghdr *)(cmsg) + 1))
#define CMSG_NXTHDR(mhdr, cmsg)  (((char *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len) + CMSG_ALIGN(sizeof(struct cmsghdr)) <= (char *)(mhdr)->msg_control + (mhdr)->msg_controllen) ? (struct cmsghdr *)((char *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len)) : (struct cmsghdr *)0)
#define CMSG_FIRSTHDR(mhdr)      ((struct cmsghdr *)((mhdr)->msg_controllen >= sizeof(struct cmsghdr) ? (mhdr)->msg_control : (void *)0))
#define CMSG_ALIGN(len)          (((len) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SOCKET_H_ */