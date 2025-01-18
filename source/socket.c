#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dlfcn.h>

/**
 * @authors 
 * @CoderRC - johndoe@example.com
 * @ilkerlovesprogramming - contact@theilker.com
 *
 * @date January 18, 2025
 * @description This file contains the implementation of a simple linked list in C.
 */
#include <stdio.h>

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

int socket(int domain, int type, int protocol) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_NOW);
    if (!Ws2_32) {
        return -1;
    }
    SOCKET (*_socket)(int domain, int type, int protocol) = dlsym(Ws2_32, "socket");
    int (*_WSAStartup)(WORD wVersionRequested, WSADATA *lpWSAData) = dlsym(Ws2_32, "WSAStartup");
    int (*_WSAGetLastError)() = dlsym(Ws2_32, "WSAGetLastError");
    if (!_socket || !_WSAStartup || !_WSAGetLastError) {
        dlclose(Ws2_32);
        return -1;
    }
    SOCKET s = _socket(domain, type, protocol);
    if (s == INVALID_SOCKET) {
        if (_WSAGetLastError() == WSANOTINITIALISED) {
            WSADATA wsaData;
            int result = _WSAStartup(0x202, &wsaData);
            if (result != 0) {
                dlclose(Ws2_32);
                return -1;
            }
            s = _socket(domain, type, protocol);
            if (s == INVALID_SOCKET) {
                dlclose(Ws2_32);
                return -1;
            }
        } else {
            dlclose(Ws2_32);
            return -1;
        }
    }
    int fd = _open_osfhandle((intptr_t)s, O_RDWR);
    if (fd == -1) {
        dlclose(Ws2_32);
        return -1;
    }
    //dlclose(Ws2_32);
    return fd;
}