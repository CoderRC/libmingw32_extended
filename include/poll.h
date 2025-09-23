#ifndef _POLL_H
#define _POLL_H

#include <time.h>

typedef unsigned long sigset_t;

#ifdef __cplusplus
extern "C" {
#endif



typedef unsigned long nfds_t;

#define POLLIN     0x001
#define POLLRDNORM 0x040
#define POLLRDBAND 0x080
#define POLLPRI    0x002
#define POLLOUT    0x004
#define POLLWRNORM 0x100
#define POLLWRBAND 0x200
#define POLLERR    0x008
#define POLLHUP    0x010
#define POLLNVAL   0x020

struct pollfd {
    int fd;
    short events;
    short revents;
};



int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int ppoll(struct pollfd fds[], nfds_t nfds,
          const struct timespec *timeout_ts,
          const sigset_t *sigmask);
int pipe(int pipefd[2]);

#ifdef __cplusplus
}
#endif

#endif /* _POLL_H */