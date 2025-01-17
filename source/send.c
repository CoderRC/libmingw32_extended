#include <sys/socket.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

ssize_t send(int socket, const void *message, size_t length, int flags) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_send) (SOCKET s, const char* buf, int len, int flags) = dlsym(Ws2_32, "send");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _send(s, message, length, flags);
    //CloseHandle(s);
    dlclose(Ws2_32);
    return result;
}