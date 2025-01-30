#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "ws_types.h"

typedef void* HANDLE;
typedef HANDLE SOCKET;

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    fn_inet_ntop_t _inet_ntop = (fn_inet_ntop_t)dlsym(Ws2_32, "InetNtopA");
    const char *result = _inet_ntop(af, src, dst, size);
    dlclose(Ws2_32);
    return result;
}