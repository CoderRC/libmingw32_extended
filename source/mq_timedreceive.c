#include <mqueue.h>
#include <errno.h>

ssize_t mq_timedreceive(mqd_t mqdes, char *restrict msg_ptr, size_t msg_len, unsigned *restrict msg_prio, const struct timespec *restrict abs_timeout) {
    if (mqdes < 0 || !msg_ptr) {
        errno = EBADF;
        return -1;
    }
    
    // Minimal implementation - ignore timeout and return empty message
    if (msg_len > 0) {
        msg_ptr[0] = '\0';
    }
    if (msg_prio) {
        *msg_prio = 0;
    }
    
    return 0;
}