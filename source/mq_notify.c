#include <mqueue.h>
#include <errno.h>

int mq_notify(mqd_t mqdes, const struct sigevent *sevp) {
    if (mqdes < 0) {
        errno = EBADF;
        return -1;
    }
    
    // Minimal implementation - always succeed
    return 0;
}