/* sys/uio.h
*/

#ifndef _SYS_UIO_H_
#define _SYS_UIO_H_

/* For size_t */
#include <stddef.h>
/* For ssize_t */
#include <sys/types.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

/*
 * Define the uio buffers used for writev, readv.
 */

struct iovec
{
  void *iov_base;
  size_t iov_len;
};

extern ssize_t readv __P ((int filedes, const struct iovec *vector, int count));
extern ssize_t writev __P ((int filedes, const struct iovec *vector, int count));

/* Read from another process' address space.  */
extern ssize_t process_vm_readv (pid_t __pid, const struct iovec *__lvec,
				 unsigned long int __liovcnt,
				 const struct iovec *__rvec,
				 unsigned long int __riovcnt,
				 unsigned long int __flags);

/* Write to another process' address space.  */
extern ssize_t process_vm_writev (pid_t __pid, const struct iovec *__lvec,
				  unsigned long int __liovcnt,
				  const struct iovec *__rvec,
				  unsigned long int __riovcnt,
				  unsigned long int __flags);

/* Flags for preadv2/pwritev2.  */
#define RWF_HIPRI	0x00000001 /* High priority request.  */
#define RWF_DSYNC	0x00000002 /* per-IO O_DSYNC.  */
#define RWF_SYNC	0x00000004 /* per-IO O_SYNC.  */
#define RWF_NOWAIT	0x00000008 /* per-IO nonblocking mode.  */
#define RWF_APPEND	0x00000010 /* per-IO O_APPEND.  */

__END_DECLS

#endif /* _SYS_UIO_H_ */
