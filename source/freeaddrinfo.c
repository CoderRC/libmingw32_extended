/* source/freeaddrinfo.c */

#include <netdb.h>
#include <dlfcn.h>
#include <stdlib.h>

void
freeaddrinfo(struct addrinfo *res)
{
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    if (!Ws2_32) {
        return;
    }

    void (*_freeaddrinfo)(struct addrinfo *) = dlsym(Ws2_32, "freeaddrinfo");
    if (_freeaddrinfo) {
        _freeaddrinfo(res);
    }

    dlclose(Ws2_32);
}