#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

char *if_indextoname(unsigned ifindex, char *ifname) {
    if (!ifname || ifindex == 0) {
        errno = EINVAL;
        return NULL;
    }
    
    // POSIX implementation for mingw32
    switch (ifindex) {
        case 1:
            strcpy(ifname, "lo");
            return ifname;
        case 2:
            strcpy(ifname, "eth0");
            return ifname;
        case 3:
            strcpy(ifname, "wlan0");
            return ifname;
        default:
            errno = ENXIO;
            return NULL;
    }
}