#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

in_addr_t inet_addr(const char *cp) {
    struct in_addr addr;
    return (in_addr_t)addr.s_addr;
}