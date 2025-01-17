#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

int socketpair(int domain, int type, int protocol, int socket_vector[2]) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_socketpair) (int af, int type, int protocol, SOCKET* sv) = dlsym(Ws2_32, "socketpair");
    SOCKET s[2];
    int result = _socketpair(domain, type, protocol, s);
    if (result == 0) {
        socket_vector[0] = _open_osfhandle((intptr_t)s[0], O_RDWR);
        socket_vector[1] = _open_osfhandle((intptr_t)s[1], O_RDWR);
    }
    dlclose(Ws2_32);
    return result;
}