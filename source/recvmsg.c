#include <sys/socket.h>
#include <windows.h>

ssize_t recvmsg(int socket, struct msghdr *message, int flags) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_recvmsg) (SOCKET s, struct msghdr* msg, int flags) = dlsym(Ws2_32, "recvmsg");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _recvmsg(s, message, flags);
    dlclose(Ws2_32);
    return result;
}