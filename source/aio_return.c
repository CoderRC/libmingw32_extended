#include <aio.h>
#include <errno.h>
#include <pthread.h>

ssize_t aio_return(struct aiocb *aiocbp) {
    if (!aiocbp) {
        errno = EINVAL;
        return -1;
    }
    
    if (aiocbp->_aio_state == 0) {
        errno = EINPROGRESS;
        return -1;
    }
    
    pthread_join(aiocbp->_aio_thread, NULL);
    
    if (aiocbp->_aio_state == 2) {
        errno = aiocbp->_aio_error;
        return -1;
    }
    
    return aiocbp->_aio_return;
}