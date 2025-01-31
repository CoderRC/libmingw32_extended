#include <arpa/inet.h>
#include <dlfcn.h>

typedef const char* (WSAAPI *inet_ntop_t)(int af, const void *src, char *dst, socklen_t size);

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    if (!Ws2_32) return NULL;
    
    inet_ntop_t _inet_ntop = (inet_ntop_t)dlsym(Ws2_32, "inet_ntop");
    const char *result = _inet_ntop ? _inet_ntop(af, src, dst, size) : NULL;
    dlclose(Ws2_32);
    return result;
}