#include <sys/socket.h>
#include <windows.h>

ssize_t sendmsg(int socket, const struct msghdr *message, int flags) {
    SOCKET s = (SOCKET)socket;
    return _sendmsg(s, message, flags);
}