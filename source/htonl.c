#include <arpa/inet.h>
#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "ws_types.h"

typedef void* HANDLE;
typedef HANDLE SOCKET;

uint32_t htonl(uint32_t hostlong) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    fn_htonl_t _htonl = (fn_htonl_t)dlsym(Ws2_32, "htonl");
    uint32_t result = _htonl(hostlong);
    dlclose(Ws2_32);
    return result;
}