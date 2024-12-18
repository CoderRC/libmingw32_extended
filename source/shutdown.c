#include <sys/socket.h>
#include <windows.h>

int shutdown(int socket, int how) {
    SOCKET s = (SOCKET)socket;
    return _shutdown(s, how);
}