#include <arpa/inet.h>
#include <dlfcn.h>

typedef unsigned long (WSAAPI *inet_addr_t)(const char *cp);

in_addr_t inet_addr(const char *cp) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    if (!Ws2_32) return INADDR_NONE;
    
    inet_addr_t _inet_addr = (inet_addr_t)dlsym(Ws2_32, "inet_addr");
    unsigned long result = _inet_addr ? _inet_addr(cp) : INADDR_NONE;
    dlclose(Ws2_32);
    return (in_addr_t)result;
}