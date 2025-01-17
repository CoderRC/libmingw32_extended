#include <sys/socket.h>
#include <windows.h>

int listen(int socket, int backlog) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_listen) (SOCKET s, int backlog) = dlsym(Ws2_32, "listen");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _listen(s, backlog);
    
    dlclose(Ws2_32);
    return result;
}