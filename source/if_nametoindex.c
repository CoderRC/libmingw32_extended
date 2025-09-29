#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

unsigned if_nametoindex(const char *ifname) {
    if (!ifname) {
        errno = EINVAL;
        return 0;
    }
    
    // POSIX implementation for mingw32
    if (strcmp(ifname, "lo") == 0) {
        return 1;
    }
    if (strcmp(ifname, "eth0") == 0) {
        return 2;
    }
    if (strcmp(ifname, "wlan0") == 0) {
        return 3;
    }
    
    errno = ENODEV;
    return 0;
}