/* sys/utsname.h
*/
#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

/* Room for a 255-character DNS host name and the terminating null */
#define _UTSNAME_LENGTH 257

  struct utsname
  {
    char sysname[_UTSNAME_LENGTH];	/* Operating system name */
    char nodename[_UTSNAME_LENGTH];	/* Network node name */
    char release[_UTSNAME_LENGTH];	/* Release level */
    char version[_UTSNAME_LENGTH];	/* Version level of the release */
    char machine[_UTSNAME_LENGTH];	/* Hardware type */
  };

/* Function prototypes */
  extern int uname (struct utsname *__name);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* sys/utsname.h */
