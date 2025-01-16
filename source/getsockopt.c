#include <sys/socket.h>
#include <windows.h>

int getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_getsockopt) (SOCKET s, int level, int optname, char* optval, int* optlen) = dlsym(Ws2_32, "getsockopt");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _getsockopt(s, level, option_name, option_value, option_len);
    CloseHandle(s);
    dlclose(Ws2_32);
    return result;
}