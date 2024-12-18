#include <sys/socket.h>
#include <windows.h>

ssize_t send(int socket, const void *message, size_t length, int flags) {
    SOCKET s = (SOCKET)socket;
    return _send(s, message, length, flags);
}