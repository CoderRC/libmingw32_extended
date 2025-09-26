#include <aio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <io.h>

static void* aio_fsync_thread(void* arg) {
    struct aiocb *aiocbp = (struct aiocb*)arg;
    
    int result = _commit(aiocbp->aio_fildes);
    
    if (result == -1) {
        aiocbp->_aio_error = errno;
        aiocbp->_aio_state = 2;
    } else {
        aiocbp->_aio_return = 0;
        aiocbp->_aio_error = 0;
        aiocbp->_aio_state = 1;
    }
    
    return NULL;
}

int aio_fsync(int op, struct aiocb *aiocbp) {
    if (!aiocbp || aiocbp->aio_fildes < 0) {
        errno = EINVAL;
        return -1;
    }
    
    // Windows doesn't distinguish O_SYNC/O_DSYNC, ignore op parameter
    aiocbp->_aio_state = 0;
    aiocbp->_aio_error = EINPROGRESS;
    
    if (pthread_create(&aiocbp->_aio_thread, NULL, aio_fsync_thread, aiocbp) != 0) {
        aiocbp->_aio_state = 2;
        aiocbp->_aio_error = errno;
        errno = EAGAIN;
        return -1;
    }
    
    return 0;
}