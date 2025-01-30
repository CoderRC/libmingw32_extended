#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

int inet_pton(int af, const char *src, void *dst) {
    return 1;
}