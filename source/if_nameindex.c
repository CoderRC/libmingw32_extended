#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct if_nameindex *if_nameindex(void) {
    // POSIX implementation for mingw32
    struct if_nameindex *list = malloc(4 * sizeof(struct if_nameindex));
    if (!list) {
        errno = ENOMEM;
        return NULL;
    }
    
    // Loopback interface
    list[0].if_index = 1;
    list[0].if_name = malloc(strlen("lo") + 1);
    if (list[0].if_name) {
        strcpy(list[0].if_name, "lo");
    }
    
    // Ethernet interface
    list[1].if_index = 2;
    list[1].if_name = malloc(strlen("eth0") + 1);
    if (list[1].if_name) {
        strcpy(list[1].if_name, "eth0");
    }
    
    // Wireless interface
    list[2].if_index = 3;
    list[2].if_name = malloc(strlen("wlan0") + 1);
    if (list[2].if_name) {
        strcpy(list[2].if_name, "wlan0");
    }
    
    // Null terminator
    list[3].if_index = 0;
    list[3].if_name = NULL;
    
    return list;
}