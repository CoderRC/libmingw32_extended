/* sys/sem.h
*/
#ifndef _SYS_SEM_H
#define _SYS_SEM_H

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

/* Semaphore operation flag */
#define SEM_UNDO 0010000	/* Set up adjust on exit entry */

/* Command definitions for semctl() */
#define GETNCNT 14		/* Get semncnt */
#define GETPID 11		/* Get sempid */
#define GETVAL 12		/* Get semval */
#define GETALL 13		/* Get all cases of semval */
#define GETZCNT 15		/* Get semzcnt */
#define SETVAL 16		/* Set semval */
#define SETALL 17		/* Set all cases of semval */

/* Implementation limits */
#define SEMMNI 128		/* Maximum number of semaphore sets */
#define SEMMSL 64		/* Maximum semaphores per set */
#define SEMOPM 64		/* Maximum operations per semop() call */
#define SEMVMX 32767		/* Maximum semaphore value */
#define SEMAEM SEMVMX		/* Maximum adjust on exit value */
#define SEMMNU 256		/* Maximum processes with SEM_UNDO entries */

  struct semid_ds
  {
    struct ipc_perm sem_perm;	/* Operation permission structure */
    unsigned short sem_nsems;	/* Number of semaphores in set */
    time_t sem_otime;		/* Last semop() time */
    time_t sem_ctime;		/* Last time changed by semctl() */
  };

  struct sembuf
  {
    unsigned short sem_num;	/* Semaphore number */
    short sem_op;		/* Semaphore operation */
    short sem_flg;		/* Operation flags */
  };

/* The fourth argument of semctl() is a union semun that the application
   defines itself, as POSIX requires:
     union semun { int val; struct semid_ds *buf; unsigned short *array; };
 */

/* Function prototypes */
  extern int semctl (int __semid, int __semnum, int __cmd, ...);
  extern int semget (key_t __key, int __nsems, int __semflg);
  extern int semop (int __semid, struct sembuf *__sops, size_t __nsops);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* sys/sem.h */
