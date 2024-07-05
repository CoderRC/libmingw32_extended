/* source/getaddrinfo.c */

#include <netdb.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short WORD;
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

int
getaddrinfo(const char *restrict nodename,
            const char *restrict servname,
            const struct addrinfo *restrict hints,
            struct addrinfo **restrict res)
{
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_NOW);
    if (!Ws2_32) {
        return EAI_FAIL;
    }

    int (*_WSAStartup)(WORD wVersionRequested, WSADATA *lpWSAData) =
        dlsym(Ws2_32, "WSAStartup");
    int (*_getaddrinfo)(const char *, const char *,
                        const struct addrinfo *, struct addrinfo **) =
        dlsym(Ws2_32, "getaddrinfo");
    int (*_WSAGetLastError)(void) = dlsym(Ws2_32, "WSAGetLastError");

    if (!_WSAStartup || !_getaddrinfo || !_WSAGetLastError) {
        dlclose(Ws2_32);
        return EAI_FAIL;
    }

    int result = _getaddrinfo(nodename, servname, hints, res);
    if (result != 0) {
        if (_WSAGetLastError() == WSANOTINITIALISED) {
            WSADATA wsaData;
            int startup_result = _WSAStartup(0x202, &wsaData);
            if (startup_result != 0) {
                dlclose(Ws2_32);
                return EAI_FAIL;
            }
            result = _getaddrinfo(nodename, servname, hints, res);
            if (result != 0) {
                dlclose(Ws2_32);
                switch (result) {
                    case 11001: return EAI_NONAME; /* WSAHOST_NOT_FOUND */
                    case 11002: return EAI_FAIL;   /* WSATRY_AGAIN */
                    case 10022: return EAI_FAIL;   /* WSAEINVAL */
                    case 10055: return EAI_MEMORY; /* WSAENOBUFS */
                    default: return EAI_FAIL;
                }
            }
        } else {
            dlclose(Ws2_32);
            switch (result) {
                case 11001: return EAI_NONAME;
                case 11002: return EAI_FAIL;
                case 10022: return EAI_FAIL;
                case 10055: return EAI_MEMORY;
                default: return EAI_FAIL;
            }
        }
    }

    dlclose(Ws2_32);
    return EAI_SUCCESS;
}