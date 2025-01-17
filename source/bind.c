#include <sys/socket.h>
#include <windows.h>

int bind(int socket, const struct sockaddr *address, socklen_t address_len) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_bind) (SOCKET s, const struct sockaddr* addr, int namelen) = dlsym(Ws2_32, "bind");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _bind(s, address, address_len);
    
    dlclose(Ws2_32);
    return result;
}