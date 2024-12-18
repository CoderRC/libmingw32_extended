#include <sys/socket.h>
#include <windows.h>

ssize_t recvmsg(int socket, struct msghdr *message, int flags) {
    SOCKET s = (SOCKET)socket;
    return _recvmsg(s, message, flags);
}