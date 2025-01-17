#include <sys/socket.h>
//#include <windows.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;
char CloseHandle(HANDLE hObject);

ssize_t recv(int socket, void *buffer, size_t length, int flags) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    int (*_recv) (SOCKET s, char* buf, int len, int flags) = dlsym(Ws2_32, "recv");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    int result = _recv(s, buffer, length, flags);
    //CloseHandle(s);
    dlclose(Ws2_32);
    return result;
}