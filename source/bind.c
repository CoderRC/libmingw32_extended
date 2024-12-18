#include <sys/socket.h>
#include <windows.h>

int bind(int socket, const struct sockaddr *address, socklen_t address_len) {
    SOCKET s = (SOCKET)socket;
    return _bind(s, address, address_len);
}