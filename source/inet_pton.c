#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "ws_types.h"

typedef void* HANDLE;
typedef HANDLE SOCKET;

int inet_pton(int af, const char *src, void *dst) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    fn_inet_pton_t _inet_pton = (fn_inet_pton_t)dlsym(Ws2_32, "InetPtonA");
    int result = _inet_pton(af, src, dst);
    dlclose(Ws2_32);
    return result;
}