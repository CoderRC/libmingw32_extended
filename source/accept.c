#include <sys/socket.h>
#include <windows.h>

int accept(int socket, struct sockaddr *address, socklen_t *address_len) {
    SOCKET s = (SOCKET)socket;
    return (int)_accept(s, address, address_len);
}