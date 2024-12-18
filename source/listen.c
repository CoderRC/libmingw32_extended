#include <sys/socket.h>
#include <windows.h>

int listen(int socket, int backlog) {
    SOCKET s = (SOCKET)socket;
    return _listen(s, backlog);
}