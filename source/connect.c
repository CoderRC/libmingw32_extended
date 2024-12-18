#include <sys/socket.h>
#include <windows.h>

int connect(int socket, const struct sockaddr *address, socklen_t address_len) {
    SOCKET s = (SOCKET)socket;
    return _connect(s, address, address_len);
}