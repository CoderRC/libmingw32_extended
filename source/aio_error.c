#include <aio.h>
#include <errno.h>

int aio_error(const struct aiocb *aiocbp) {
    if (!aiocbp) {
        return EINVAL;
    }
    return aiocbp->_aio_error;
}