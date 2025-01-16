#include <sys/socket.h>
#include <windows.h>

ssize_t recvfrom(int socket, void *buffer, size_t length, int flags,
                 struct sockaddr *address, socklen_t *address_len) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_recvfrom) (SOCKET s, char* buf, int len, int flags,
                      struct sockaddr* from, int* fromlen) = dlsym(Ws2_32, "recvfrom");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _recvfrom(s, buffer, length, flags, address, address_len);
    CloseHandle(s);
    dlclose(Ws2_32);
    return result;
}