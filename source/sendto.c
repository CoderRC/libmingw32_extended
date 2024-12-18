#include <sys/socket.h>
#include <windows.h>

ssize_t sendto(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len) {
    SOCKET s = (SOCKET)socket;
    return _sendto(s, message, length, flags, dest_addr, dest_len);
}