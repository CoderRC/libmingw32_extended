#ifndef _WS_TYPES_H
#define _WS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Function pointer types for WinSock functions */
typedef unsigned long (*fn_htonl_t)(unsigned long);
typedef unsigned short (*fn_htons_t)(unsigned short);
typedef unsigned long (*fn_ntohl_t)(unsigned long);
typedef unsigned short (*fn_ntohs_t)(unsigned short);
typedef unsigned long (*fn_inet_addr_t)(const char*);
typedef char* (*fn_inet_ntoa_t)(struct in_addr);
typedef const char* (*fn_inet_ntop_t)(int, const void*, char*, size_t);
typedef int (*fn_inet_pton_t)(int, const char*, void*);

#ifdef __cplusplus
}
#endif

#endif /* _WS_TYPES_H */