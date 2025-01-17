#include <sys/socket.h>
//#include <windows.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;
char CloseHandle(HANDLE hObject);

int accept(int socket, struct sockaddr *address, socklen_t *address_len) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    SOCKET (*_accept) (SOCKET s, struct sockaddr* addr, int* addrlen) = dlsym(Ws2_32, "accept");
    SOCKET s = (SOCKET)_get_osfhandle(socket);
    SOCKET out = _accept(s, address, address_len);
    //CloseHandle(s);
    dlclose(Ws2_32); // reference count decrement once it is zero then all sockets are closed
    return _open_osfhandle((intptr_t)out, O_RDWR);
}