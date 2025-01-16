#include <sys/socket.h>
#include <windows.h>
#include <fcntl.h>

static SOCKET (*_socket)(int domain, int type, int protocol);
static void *ws2_32_handle = NULL;

int socket(int domain, int type, int protocol) {
    if (!ws2_32_handle) {
        ws2_32_handle = dlopen("ws2_32.dll", RTLD_LAZY);
    }
    if (!_socket) {
        _socket = (SOCKET (*)(int, int, int))dlsym(ws2_32_handle, "socket");
    }
    SOCKET s = _socket(domain, type, protocol);
    return (int)s;
}