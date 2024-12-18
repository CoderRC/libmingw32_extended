#include <sys/socket.h>
#include <windows.h>
#include <fcntl.h>

int socket(int domain, int type, int protocol) {
    SOCKET s = _socket(domain, type, protocol);
    return _open_osfhandle((intptr_t)s, O_RDWR);
}