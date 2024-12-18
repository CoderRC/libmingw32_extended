#include <sys/socket.h>
#include <windows.h>

int getsockname(int socket, struct sockaddr *address, socklen_t *address_len) {
    SOCKET s = (SOCKET)socket;
    return _getsockname(s, address, address_len);
}