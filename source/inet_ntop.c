#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
    return dst;
}