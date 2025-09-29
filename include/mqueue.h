#ifndef _MQUEUE_H
#define _MQUEUE_H

#include <stddef.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Message queue descriptor type */
typedef int mqd_t;

#ifndef _SSIZE_T_DEFINED
typedef long ssize_t;
#define _SSIZE_T_DEFINED
#endif

/* mode_t is defined in sys/types.h */

/* Message queue attributes structure */
struct mq_attr {
    long mq_flags;    /* Message queue flags (O_NONBLOCK) */
    long mq_maxmsg;   /* Maximum number of messages */
    long mq_msgsize;  /* Maximum message size */
    long mq_curmsgs;  /* Number of messages currently queued */
    long __reserved[4]; /* Reserved for future use */
};

/* Signal value union */
union sigval {
    int sival_int;      /* Integer value */
    void *sival_ptr;    /* Pointer value */
};

/* Signal event structure for mq_notify */
struct sigevent {
    int sigev_notify;               /* Notification method */
    int sigev_signo;                /* Signal number */
    union sigval sigev_value;       /* Signal value */
    void (*sigev_notify_function)(union sigval);
    pthread_attr_t *sigev_notify_attributes;
};

/* Notification methods */
#define SIGEV_NONE      0   /* No notification */
#define SIGEV_SIGNAL    1   /* Signal notification */
#define SIGEV_THREAD    2   /* Thread notification */

/* File control flags */
#ifndef O_RDONLY
#define O_RDONLY    0x0000  /* Read only */
#define O_WRONLY    0x0001  /* Write only */
#define O_RDWR      0x0002  /* Read/write */
#define O_CREAT     0x0100  /* Create if nonexistent */
#define O_EXCL      0x0200  /* Exclusive use flag */
#define O_NONBLOCK  0x0800  /* Non-blocking flag */
#define O_TRUNC     0x0400  /* Truncate flag */
#endif

/* Message queue limits */
#define MQ_OPEN_MAX     8       /* Maximum number of open message queues */
#define MQ_PRIO_MAX     32      /* Maximum message priority */
#define MQ_MSGSIZE_MAX  8192    /* Maximum message size */
#define MQ_MAXMSG_MAX   32      /* Maximum messages in queue */

/* Error return value */
#define MQD_INVALID     ((mqd_t)-1)

/* Function prototypes */
int      mq_close(mqd_t mqdes);
int      mq_getattr(mqd_t mqdes, struct mq_attr *mqstat);
int      mq_notify(mqd_t mqdes, const struct sigevent *sevp);
mqd_t    mq_open(const char *name, int oflag, ...);
ssize_t  mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
int      mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
int      mq_setattr(mqd_t mqdes, const struct mq_attr *restrict mqstat, struct mq_attr *restrict omqstat);
ssize_t  mq_timedreceive(mqd_t mqdes, char *restrict msg_ptr, size_t msg_len, unsigned *restrict msg_prio, const struct timespec *restrict abs_timeout);
int      mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abs_timeout);
int      mq_unlink(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* _MQUEUE_H */