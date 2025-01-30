#include <arpa/inet.h>
#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

uint32_t htonl(uint32_t hostlong) {
    return hostlong;
}