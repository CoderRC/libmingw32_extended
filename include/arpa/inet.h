#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <inttypes.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Types required by POSIX */
  typedef uint16_t in_port_t;
  typedef uint32_t in_addr_t;

/* Address-to-string buffer sizes */
#define INET_ADDRSTRLEN 16
#ifdef _IPV6_
#define INET6_ADDRSTRLEN 46
#endif

/* Convert values between host and network byte order */
  uint32_t htonl (uint32_t);
  uint16_t htons (uint16_t);
  uint32_t ntohl (uint32_t);
  uint16_t ntohs (uint16_t);

/* Obsolescent functions */
#if !defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
  in_addr_t inet_addr (const char *);
  char *inet_ntoa (struct in_addr);
#endif

/* Current standard functions */
  const char *inet_ntop (int, const void *restrict, char *restrict,
			 socklen_t);
  int inet_pton (int, const char *restrict, void *restrict);

#ifdef __cplusplus
}
#endif

#endif				/* _ARPA_INET_H */
