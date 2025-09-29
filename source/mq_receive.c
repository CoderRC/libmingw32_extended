#include <mqueue.h>
#include <errno.h>
#include <string.h>

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    if (mqdes < 0 || !msg_ptr) {
        errno = EBADF;
        return -1;
    }
    
    // Minimal implementation - return empty message
    if (msg_len > 0) {
        msg_ptr[0] = '\0';
    }
    if (msg_prio) {
        *msg_prio = 0;
    }
    
    return 0;
}