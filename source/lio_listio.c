#include <aio.h>
#include <errno.h>
#include <pthread.h>

int lio_listio(int mode, struct aiocb *restrict const list[restrict], int nent, struct sigevent *restrict sig) {
    if (!list || nent <= 0 || (mode != LIO_WAIT && mode != LIO_NOWAIT)) {
        errno = EINVAL;
        return -1;
    }
    
    int error_count = 0;
    
    for (int i = 0; i < nent; i++) {
        if (!list[i]) continue;
        
        int result = 0;
        switch (list[i]->aio_lio_opcode) {
            case LIO_READ:
                result = aio_read(list[i]);
                break;
            case LIO_WRITE:
                result = aio_write(list[i]);
                break;
            case LIO_NOP:
                list[i]->_aio_state = 1;
                list[i]->_aio_error = 0;
                list[i]->_aio_return = 0;
                break;
            default:
                errno = EINVAL;
                return -1;
        }
        
        if (result == -1) {
            error_count++;
        }
    }
    
    if (mode == LIO_WAIT) {
        for (int i = 0; i < nent; i++) {
            if (list[i] && list[i]->aio_lio_opcode != LIO_NOP) {
                pthread_join(list[i]->_aio_thread, NULL);
            }
        }
    }
    
    return error_count > 0 ? -1 : 0;
}