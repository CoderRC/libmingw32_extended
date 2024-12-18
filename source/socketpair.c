#include <sys/socket.h>
#include <windows.h>
#include <fcntl.h>

int socketpair(int domain, int type, int protocol, int socket_vector[2]) {
    SOCKET s[2];
    int result = _socketpair(domain, type, protocol, s);
    if (result == 0) {
        socket_vector[0] = _open_osfhandle((intptr_t)s[0], O_RDWR);
        socket_vector[1] = _open_osfhandle((intptr_t)s[1], O_RDWR);
    }
    return result;
}