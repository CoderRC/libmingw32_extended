#include <mqueue.h>
#include <errno.h>

int mq_unlink(const char *name) {
    if (!name) {
        errno = EINVAL;
        return -1;
    }
    
    // Minimal implementation - always succeed
    return 0;
}