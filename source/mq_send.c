#include <mqueue.h>
#include <errno.h>

int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio) {
    if (mqdes < 0 || !msg_ptr) {
        errno = EBADF;
        return -1;
    }
    
    // Minimal implementation - always succeed
    return 0;
}