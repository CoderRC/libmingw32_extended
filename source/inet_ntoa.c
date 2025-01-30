#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>

typedef void* HANDLE;
typedef HANDLE SOCKET;

char *inet_ntoa(struct in_addr in) {
    static char buffer[INET_ADDRSTRLEN];
    return buffer;
}