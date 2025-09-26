#ifndef _AIO_H
#define _AIO_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#define _SSIZE_T_DEFINED
#endif

#ifndef _OFF_T_DEFINED
typedef long long off_t;
#define _OFF_T_DEFINED
#endif

union sigval {
    int sival_int;
    void *sival_ptr;
};

struct sigevent {
    int sigev_notify;
    int sigev_signo;
    union sigval sigev_value;
    void (*sigev_notify_function)(union sigval);
};

struct aiocb {
    int             aio_fildes;     /* File descriptor */
    off_t           aio_offset;     /* File offset */
    volatile void  *aio_buf;        /* Location of buffer */
    size_t          aio_nbytes;     /* Length of transfer */
    int             aio_reqprio;    /* Request priority offset */
    struct sigevent aio_sigevent;   /* Signal number and value */
    int             aio_lio_opcode; /* Operation to be performed */
    
    /* Internal fields */
    pthread_t       _aio_thread;
    int             _aio_error;
    ssize_t         _aio_return;
    int             _aio_state;     /* 0=pending, 1=done, 2=error */
};

/* Return values for aio_cancel() */
#define AIO_ALLDONE     1
#define AIO_CANCELED    2
#define AIO_NOTCANCELED 3

/* lio_listio() operation options */
#define LIO_NOP    0
#define LIO_READ   1
#define LIO_WRITE  2

/* lio_listio() synchronization options */
#define LIO_NOWAIT 0
#define LIO_WAIT   1

/* Function prototypes */
int      aio_cancel(int, struct aiocb *);
int      aio_error(const struct aiocb *);
int      aio_fsync(int, struct aiocb *);
int      aio_read(struct aiocb *);
ssize_t  aio_return(struct aiocb *);
int      aio_suspend(const struct aiocb *const [], int, const struct timespec *);
int      aio_write(struct aiocb *);
int      lio_listio(int, struct aiocb *restrict const [restrict], int, struct sigevent *restrict);

#ifdef __cplusplus
}
#endif

#endif /* _AIO_H */