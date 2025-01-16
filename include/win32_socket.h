#ifndef _WIN32_SOCKET_H_
#define _WIN32_SOCKET_H_

typedef unsigned short WORD;
typedef void* HANDLE;
typedef HANDLE SOCKET;
#define INVALID_SOCKET  (SOCKET)(-1)
#define WSANOTINITIALISED 10093

typedef struct _WSADATA {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[256];
    char szSystemStatus[128];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char *lpVendorInfo;
} WSADATA;

#endif /* _WIN32_SOCKET_H_ */