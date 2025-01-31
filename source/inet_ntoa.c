#include <arpa/inet.h>
#include <dlfcn.h>

typedef char* (WSAAPI *inet_ntoa_t)(struct in_addr in);

char *inet_ntoa(struct in_addr in) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    if (!Ws2_32) return NULL;
    
    inet_ntoa_t _inet_ntoa = (inet_ntoa_t)dlsym(Ws2_32, "inet_ntoa");
    char *result = _inet_ntoa ? _inet_ntoa(in) : NULL;
    dlclose(Ws2_32);
    return result;
}