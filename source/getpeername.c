#include <sys/socket.h>
#include <windows.h>

int getpeername(int socket, struct sockaddr *address, socklen_t *address_len) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_getpeername) (SOCKET s, struct sockaddr* addr, int* namelen) = dlsym(Ws2_32, "getpeername");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _getpeername(s, address, address_len);
    CloseHandle(s);
    dlclose(Ws2_32);
    return result;
}