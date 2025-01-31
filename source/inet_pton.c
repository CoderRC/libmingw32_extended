#include <arpa/inet.h>
#include <dlfcn.h>

typedef int (WSAAPI *inet_pton_t)(int af, const char *src, void *dst);

int inet_pton(int af, const char *src, void *dst) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    if (!Ws2_32) return -1;
    
    inet_pton_t _inet_pton = (inet_pton_t)dlsym(Ws2_32, "inet_pton");
    int result = _inet_pton ? _inet_pton(af, src, dst) : -1;
    dlclose(Ws2_32);
    return result;
}