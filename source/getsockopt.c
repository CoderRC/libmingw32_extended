#include <sys/socket.h>
#include <windows.h>

int getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len) {
    SOCKET s = (SOCKET)socket;
    return _getsockopt(s, level, option_name, option_value, option_len);
}