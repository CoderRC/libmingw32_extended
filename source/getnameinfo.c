#include <netdb.h>
#include <dlfcn.h>
#include <string.h>

#ifndef WSAAPI
#define WSAAPI __stdcall
#endif

typedef int (WSAAPI *GETNAMEINFO)(const struct sockaddr *addr, socklen_t addrlen,
                                  char *host, socklen_t hostlen,
                                  char *serv, socklen_t servlen, int flags);

int getnameinfo(const struct sockaddr *restrict addr, socklen_t addrlen,
                char *restrict host, socklen_t hostlen,
                char *restrict serv, socklen_t servlen, int flags) {
    static void *ws2_32 = NULL;
    static GETNAMEINFO p_getnameinfo = NULL;
    
    if (!ws2_32) {
        ws2_32 = dlopen("ws2_32.dll", RTLD_NOW);
        if (!ws2_32) return EAI_FAIL;
        
        p_getnameinfo = (GETNAMEINFO)dlsym(ws2_32, "GetNameInfoA");
        if (!p_getnameinfo) {
            dlclose(ws2_32);
            ws2_32 = NULL;
            return EAI_FAIL;
        }
    }
    
    return p_getnameinfo(addr, addrlen, host, hostlen, serv, servlen, flags);
}