#include <sys/socket.h>
#include <windows.h>

static int (*_accept)(SOCKET s, struct sockaddr* address, socklen_t* address_len);
static void *ws2_32_handle = NULL;

int accept(int socket, struct sockaddr *address, socklen_t *address_len) {
    if (!ws2_32_handle) {
        ws2_32_handle = dlopen("ws2_32.dll", RTLD_LAZY);
    }
    if (!_accept) {
        _accept = (int (*)(SOCKET, struct sockaddr*, socklen_t*))dlsym(ws2_32_handle, "accept");
    }
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int out = _accept(s, address, address_len);
    CloseHandle(s);
    return out;
}