#include <mqueue.h>
#include <errno.h>

int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat) {
    if (mqdes < 0 || !mqstat) {
        errno = EBADF;
        return -1;
    }
    
    // Minimal implementation - return default attributes
    mqstat->mq_flags = 0;
    mqstat->mq_maxmsg = 10;
    mqstat->mq_msgsize = 8192;
    mqstat->mq_curmsgs = 0;
    
    return 0;
}