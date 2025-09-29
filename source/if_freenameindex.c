#include <net/if.h>
#include <stdlib.h>

void if_freenameindex(struct if_nameindex *ptr) {
    if (!ptr) return;
    
    for (int i = 0; ptr[i].if_index != 0 || ptr[i].if_name != NULL; i++) {
        if (ptr[i].if_name) {
            free(ptr[i].if_name);
            ptr[i].if_name = NULL;
        }
        if (ptr[i].if_index == 0) break;
    }
    free(ptr);
}