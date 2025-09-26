#include <aio.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

int aio_suspend(const struct aiocb *const list[], int nent, const struct timespec *timeout) {
    if (!list || nent <= 0) {
        errno = EINVAL;
        return -1;
    }
    
    struct timespec start_time, current_time;
    if (timeout && clock_gettime(CLOCK_REALTIME, &start_time) == -1) {
        return -1;
    }
    
    while (1) {
        for (int i = 0; i < nent; i++) {
            if (list[i] && list[i]->_aio_state != 0) {
                return 0;
            }
        }
        
        if (timeout) {
            if (clock_gettime(CLOCK_REALTIME, &current_time) == -1) {
                return -1;
            }
            
            long elapsed_ns = (current_time.tv_sec - start_time.tv_sec) * 1000000000L +
                             (current_time.tv_nsec - start_time.tv_nsec);
            long timeout_ns = timeout->tv_sec * 1000000000L + timeout->tv_nsec;
            
            if (elapsed_ns >= timeout_ns) {
                errno = EAGAIN;
                return -1;
            }
        }
        
        usleep(1000);
    }
}