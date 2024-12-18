#include <sys/socket.h>
#include <windows.h>

int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len) {
    SOCKET s = (SOCKET)socket;
    return _setsockopt(s, level, option_name, option_value, option_len);
}