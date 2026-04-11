/* pwd.h
*/
#ifndef _PWD_H
#define _PWD_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#ifndef _UID_T_DEFINED
  typedef unsigned int uid_t;
#define _UID_T_DEFINED
#endif

#ifndef _GID_T_DEFINED
  typedef unsigned int gid_t;
#define _GID_T_DEFINED
#endif

  struct passwd
  {
    char *pw_name;		/* User's login name */
    uid_t pw_uid;		/* Numerical user ID */
    gid_t pw_gid;		/* Numerical group ID */
    char *pw_dir;		/* Initial working directory */
    char *pw_shell;		/* Program to use as shell */
  };

/* Function prototypes */
  extern void endpwent (void);
  extern struct passwd *getpwent (void);
  extern struct passwd *getpwnam (const char *__name);
  extern int getpwnam_r (const char *__restrict __name,
			 struct passwd *__restrict __resultbuf,
			 char *__restrict __buffer, size_t __buflen,
			 struct passwd **__restrict __result);
  extern struct passwd *getpwuid (uid_t __uid);
  extern int getpwuid_r (uid_t __uid, struct passwd *__restrict __resultbuf,
			 char *__restrict __buffer, size_t __buflen,
			 struct passwd **__restrict __result);
  extern void setpwent (void);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* pwd.h */
