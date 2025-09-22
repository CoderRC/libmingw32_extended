/* spawn.h
*/
#ifndef _SPAWN_H
#ifdef __argv
#define _SPAWN_H __argv
#undef __argv
#endif

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#include <sched.h>
#include <sys/types.h>
#include <signal.h>

/* Flags for posix_spawnattr_t. */
#define POSIX_SPAWN_RESETIDS 0x01
#define POSIX_SPAWN_SETPGROUP 0x02
#define POSIX_SPAWN_SETSIGDEF 0x04
#define POSIX_SPAWN_SETSIGMASK 0x08
#define POSIX_SPAWN_SETSCHEDPARAM 0x10
#define POSIX_SPAWN_SETSCHEDULER 0x20
#ifndef POSIX_SPAWN_USEVFORK
#define POSIX_SPAWN_USEVFORK 0x40
#endif
#ifndef POSIX_SPAWN_SETSID
#define POSIX_SPAWN_SETSID 0x80
#endif

  typedef struct
  {
    short int __flags;
    pid_t __pgrp;
    void *__sd;
    void *__ss;
    struct sched_param __sp;
    int __policy;
  } posix_spawnattr_t;

  typedef struct
  {
    int __allocated;
    int __used;
    struct __spawn_action *__actions;
  } posix_spawn_file_actions_t;

/* spawn process */
  extern int posix_spawn (pid_t * __restrict __pid,
			  const char *__restrict __path,
			  const posix_spawn_file_actions_t *
			  __restrict __file_actions,
			  const posix_spawnattr_t * __restrict __attrp,
			  char *const __argv[], char *const __envp[]);
  extern int posix_spawnp (pid_t * __pid, const char *__file,
			   const posix_spawn_file_actions_t * __file_actions,
			   const posix_spawnattr_t * __attrp,
			   char *const __argv[], char *const __envp[]);

/* file descriptor calls before spawn and before file descriptor calls reset after spawn */
  extern int posix_spawn_file_actions_init (posix_spawn_file_actions_t *
					    __file_actions);
  extern int posix_spawn_file_actions_destroy (posix_spawn_file_actions_t *
					       __file_actions);
  extern int posix_spawn_file_actions_addopen (posix_spawn_file_actions_t *
					       __restrict __file_actions,
					       int __fd,
					       const char *__restrict __path,
					       int __oflag, mode_t __mode);
  extern int posix_spawn_file_actions_addclose (posix_spawn_file_actions_t *
						__file_actions, int __fd);
  extern int posix_spawn_file_actions_adddup2 (posix_spawn_file_actions_t *
					       __file_actions,
					       int __fd, int __newfd);

/* spawn attributes */

  extern int posix_spawnattr_init (posix_spawnattr_t * __attr);
  extern int posix_spawnattr_destroy (posix_spawnattr_t * __attr);
  extern int posix_spawnattr_getsigdefault (const posix_spawnattr_t *
					    __restrict __attr,
					    void *__restrict __sigdefault);
  extern int posix_spawnattr_setsigdefault (posix_spawnattr_t *
					    __restrict __attr,
					    const void *__restrict
					    __sigdefault);
  extern int posix_spawnattr_getsigmask (const posix_spawnattr_t *
					 __restrict __attr,
					 void *__restrict __sigmask);
  extern int posix_spawnattr_setsigmask (posix_spawnattr_t *
					 __restrict __attr,
					 const void *__restrict __sigmask);
  extern int posix_spawnattr_getflags (const posix_spawnattr_t *
				       __restrict __attr,
				       short int *__restrict __flags);
  extern int posix_spawnattr_setflags (posix_spawnattr_t * _attr,
				       short int __flags);
  extern int posix_spawnattr_getpgroup (const posix_spawnattr_t *
					__restrict __attr,
					pid_t * __restrict __pgroup);
  extern int posix_spawnattr_setpgroup (posix_spawnattr_t * __attr,
					pid_t __pgroup);
  extern int posix_spawnattr_getschedpolicy (const posix_spawnattr_t *
					     __restrict __attr,
					     int *__restrict __schedpolicy);
  extern int posix_spawnattr_setschedpolicy (posix_spawnattr_t * __attr,
					     int __schedpolicy);
  extern int posix_spawnattr_getschedparam (const posix_spawnattr_t *
					    __restrict __attr,
					    struct sched_param *__restrict
					    __schedparam);
  extern int posix_spawnattr_setschedparam (posix_spawnattr_t *
					    __restrict __attr,
					    const struct sched_param
					    *__restrict __schedparam);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#ifdef _SPAWN_H
#define __argv _SPAWN_H
#else
#define _SPAWN_H
#endif

#endif				/* spawn.h */
