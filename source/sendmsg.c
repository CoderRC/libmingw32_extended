#include <sys/socket.h>
#include <windows.h>

ssize_t sendmsg(int socket, const struct msghdr *message, int flags) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_sendmsg) (SOCKET s, const struct msghdr* msg, int flags) = dlsym(Ws2_32, "sendmsg");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _sendmsg(s, message, flags);
    CloseHandle(s);
    dlclose(Ws2_32);
    return result;
}