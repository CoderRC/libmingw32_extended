#include <aio.h>
#include <errno.h>
#include <pthread.h>

int aio_cancel(int fildes, struct aiocb *aiocbp) {
    if (aiocbp) {
        if (aiocbp->_aio_state == 1) {
            return AIO_ALLDONE;
        }
        if (pthread_cancel(aiocbp->_aio_thread) == 0) {
            aiocbp->_aio_state = 2;
            aiocbp->_aio_error = ECANCELED;
            return AIO_CANCELED;
        }
        return AIO_NOTCANCELED;
    }
    return AIO_ALLDONE;
}