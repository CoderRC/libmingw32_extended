#include <sys/socket.h>
#include <windows.h>

ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
                 struct sockaddr *address, socklen_t *address_len) {
    SOCKET s = (SOCKET)socket;
    return _recvfrom(s, buffer, length, flags, address, address_len);
}