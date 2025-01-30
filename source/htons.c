#include <arpa/inet.h>
#include <stdint.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

uint16_t htons(uint16_t hostshort) {
    return hostshort;
}