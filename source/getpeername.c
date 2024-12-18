#include <sys/socket.h>
#include <windows.h>

int getpeername(int socket, struct sockaddr *address, socklen_t *address_len) {
    SOCKET s = (SOCKET)socket;
    return _getpeername(s, address, address_len);
}