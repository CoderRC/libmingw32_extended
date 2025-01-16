#include <sys/socket.h>
#include <windows.h>

ssize_t sendto(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_sendto) (SOCKET s, const char* buf, int len, int flags,
                    const struct sockaddr* to, int tolen) = dlsym(Ws2_32, "sendto");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _sendto(s, message, length, flags, dest_addr, dest_len);
    CloseHandle(s);
    dlclose(Ws2_32);
    return result;
}