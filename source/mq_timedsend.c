#include <mqueue.h>
#include <errno.h>

int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abs_timeout) {
    if (mqdes < 0 || !msg_ptr) {
        errno = EBADF;
        return -1;
    }
    
    // Minimal implementation - ignore timeout and always succeed
    return 0;
}