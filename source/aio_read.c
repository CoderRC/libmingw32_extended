#include <aio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

static void* aio_read_thread(void* arg) {
    struct aiocb *aiocbp = (struct aiocb*)arg;
    
    off_t old_offset = lseek(aiocbp->aio_fildes, 0, SEEK_CUR);
    if (old_offset == -1) {
        aiocbp->_aio_error = errno;
        aiocbp->_aio_state = 2;
        return NULL;
    }
    
    if (lseek(aiocbp->aio_fildes, aiocbp->aio_offset, SEEK_SET) == -1) {
        aiocbp->_aio_error = errno;
        aiocbp->_aio_state = 2;
        return NULL;
    }
    
    ssize_t result = read(aiocbp->aio_fildes, (void*)aiocbp->aio_buf, aiocbp->aio_nbytes);
    lseek(aiocbp->aio_fildes, old_offset, SEEK_SET);
    
    if (result == -1) {
        aiocbp->_aio_error = errno;
        aiocbp->_aio_state = 2;
    } else {
        aiocbp->_aio_return = result;
        aiocbp->_aio_error = 0;
        aiocbp->_aio_state = 1;
    }
    
    return NULL;
}

int aio_read(struct aiocb *aiocbp) {
    if (!aiocbp || aiocbp->aio_fildes < 0) {
        errno = EINVAL;
        return -1;
    }
    
    aiocbp->_aio_state = 0;
    aiocbp->_aio_error = EINPROGRESS;
    
    if (pthread_create(&aiocbp->_aio_thread, NULL, aio_read_thread, aiocbp) != 0) {
        aiocbp->_aio_state = 2;
        aiocbp->_aio_error = errno;
        errno = EAGAIN;
        return -1;
    }
    
    return 0;
}