/* sys/shm.h
*/
#ifndef _SYS_SHM_H
#define _SYS_SHM_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

/* Same uid_t as pwd.h, defined before sys/ipc.h so struct ipc_perm
   agrees with struct passwd whichever header is included first. */
#ifndef _UID_T_DEFINED
  typedef pid_t uid_t;
#define _UID_T_DEFINED
#endif

#ifndef _GID_T_DEFINED
  typedef unsigned int gid_t;
#define _GID_T_DEFINED
#endif

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#include <sys/ipc.h>

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

  typedef unsigned int shmatt_t;	/* Number of current attaches */

/* Segment low boundary address multiple: the Windows allocation
   granularity, which MapViewOfFileEx requires */
#define SHMLBA 65536

/* Attach flags */
#define SHM_RDONLY 0010000	/* Attach read-only */
#define SHM_RND 0020000		/* Round attach address to SHMLBA */

/* Implementation limits */
#define SHMMIN 1		/* Minimum segment size in bytes */
#define SHMMAX 0x40000000	/* Maximum segment size in bytes (1 GiB) */
#define SHMMNI 128		/* Maximum number of segments */
#define SHMSEG 128		/* Maximum attaches per process */

  struct shmid_ds
  {
    struct ipc_perm shm_perm;	/* Operation permission structure */
    size_t shm_segsz;		/* Size of segment in bytes */
    pid_t shm_lpid;		/* Process ID of last shared memory operation */
    pid_t shm_cpid;		/* Process ID of creator */
    shmatt_t shm_nattch;	/* Number of current attaches */
    time_t shm_atime;		/* Time of last shmat() */
    time_t shm_dtime;		/* Time of last shmdt() */
    time_t shm_ctime;		/* Time of last change by shmctl() */
  };

/* Function prototypes */
  extern void *shmat (int __shmid, const void *__shmaddr, int __shmflg);
  extern int shmctl (int __shmid, int __cmd, struct shmid_ds *__buf);
  extern int shmdt (const void *__shmaddr);
  extern int shmget (key_t __key, size_t __size, int __shmflg);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* sys/shm.h */
