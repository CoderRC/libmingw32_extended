/* poll.h
*/

#ifndef _POLL_H
#define _POLL_H

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#include <sys/types.h>
#include <time.h>

/* sigset_t as in signal.h. MinGW's sys/types.h only defines it when
   _POSIX is set, so take the same _sigset_t it uses instead of a type
   of our own that would conflict. */
#ifndef _POSIX
#ifndef _SIGSET_T_DEFINED
#define _SIGSET_T_DEFINED
  typedef _sigset_t sigset_t;
#endif
#endif

#ifndef _NFDS_T_DEFINED
#define _NFDS_T_DEFINED
  typedef unsigned long nfds_t;
#endif

/* Event flags for events and revents */
#define POLLIN 0x001		/* Data other than high-priority may be read */
#define POLLRDNORM 0x040	/* Normal data may be read */
#define POLLRDBAND 0x080	/* Priority data may be read */
#define POLLPRI 0x002		/* High-priority data may be read */
#define POLLOUT 0x004		/* Normal data may be written */
#define POLLWRNORM 0x100	/* Equivalent to POLLOUT */
#define POLLWRBAND 0x200	/* Priority data may be written */
#define POLLERR 0x008		/* An error has occurred (revents only) */
#define POLLHUP 0x010		/* Device has been disconnected (revents only) */
#define POLLNVAL 0x020		/* Invalid fd member (revents only) */

  struct pollfd
  {
    int fd;			/* Descriptor being polled */
    short events;		/* The input event flags */
    short revents;		/* The output event flags */
  };

/* poll */
  extern int poll (struct pollfd __fds[], nfds_t __nfds, int __timeout);
  extern int ppoll (struct pollfd __fds[], nfds_t __nfds,
		    const struct timespec *__restrict __timeout,
		    const sigset_t * __restrict __sigmask);

/* Not part of POSIX poll.h (it belongs in unistd.h); kept here so code
   that relies on it from this header still compiles. */
  extern int pipe (int __fildes[2]);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* _POLL_H */
