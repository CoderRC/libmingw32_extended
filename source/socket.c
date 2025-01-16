#include <sys/socket.h>
#include "win32_socket.h"
#include <fcntl.h>
#include <dlfcn.h>

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
    dlclose(Ws2_32);
    return fd;
}