#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "ws_types.h"

typedef void* HANDLE;
typedef HANDLE SOCKET;

char *inet_ntoa(struct in_addr in) {
    void *Ws2_32 = dlopen("ws2_32.dll", RTLD_LAZY);
    fn_inet_ntoa_t _inet_ntoa = (fn_inet_ntoa_t)dlsym(Ws2_32, "inet_ntoa");
    char *result = _inet_ntoa(in);
    static char buffer[INET_ADDRSTRLEN];
    if (result) {
        strncpy(buffer, result, INET_ADDRSTRLEN - 1);
        buffer[INET_ADDRSTRLEN - 1] = '\0';
    } else {
        buffer[0] = '\0';
    }
    dlclose(Ws2_32);
    return buffer;
}