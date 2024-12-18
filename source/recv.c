#include <sys/socket.h>
#include <windows.h>

ssize_t recv(int socket, void *buffer, size_t length, int flags) {
    SOCKET s = (SOCKET)socket;
    return _recv(s, buffer, length, flags);
}