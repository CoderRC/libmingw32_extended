#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "ws_types.h"

typedef void* HANDLE;
typedef HANDLE SOCKET;

in_addr_t inet_addr(const char *cp) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    fn_inet_addr_t _inet_addr = (fn_inet_addr_t)dlsym(Ws2_32, "inet_addr");
    unsigned long result = _inet_addr(cp);
    dlclose(Ws2_32);
    return (in_addr_t)result;
}