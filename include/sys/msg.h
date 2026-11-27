/* sys/msg.h
*/
#ifndef _SYS_MSG_H
#define _SYS_MSG_H

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

#ifndef _SSIZE_T_DEFINED
  typedef long ssize_t;
#define _SSIZE_T_DEFINED
#endif

  typedef unsigned int msgqnum_t;	/* Number of messages in a queue */
  typedef unsigned int msglen_t;	/* Number of bytes allowed in a queue */

/* Message operation flag */
#define MSG_NOERROR 0010000	/* No error if big message */

/* Implementation limits */
#define MSGMNI 32		/* Maximum number of message queues */
#define MSGMAX 8192		/* Maximum size of one message, in bytes */
#define MSGMNB 16384		/* Default and maximum msg_qbytes */
#define MSGTQL 512		/* Maximum number of messages in one queue */

  struct msqid_ds
  {
    struct ipc_perm msg_perm;	/* Operation permission structure */
    msgqnum_t msg_qnum;		/* Number of messages currently on queue */
    msglen_t msg_qbytes;	/* Maximum number of bytes allowed on queue */
    pid_t msg_lspid;		/* Process ID of last msgsnd() */
    pid_t msg_lrpid;		/* Process ID of last msgrcv() */
    time_t msg_stime;		/* Time of last msgsnd() */
    time_t msg_rtime;		/* Time of last msgrcv() */
    time_t msg_ctime;		/* Time of last change */
  };

/* Function prototypes */
  extern int msgctl (int __msqid, int __cmd, struct msqid_ds *__buf);
  extern int msgget (key_t __key, int __msgflg);
  extern ssize_t msgrcv (int __msqid, void *__msgp, size_t __msgsz,
			 long __msgtyp, int __msgflg);
  extern int msgsnd (int __msqid, const void *__msgp, size_t __msgsz,
		     int __msgflg);

#ifdef __cplusplus
}
#endif				/* __cplusplus */

#endif				/* sys/msg.h */
