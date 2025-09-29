#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Types are defined in sys/types.h */
#ifndef _KEY_T_DEFINED
typedef int key_t;
#define _KEY_T_DEFINED
#endif

#ifndef _UID_T_DEFINED
typedef unsigned int uid_t;
#define _UID_T_DEFINED
#endif

#ifndef _GID_T_DEFINED
typedef unsigned int gid_t;
#define _GID_T_DEFINED
#endif

/* IPC permission structure */
struct ipc_perm {
    uid_t    uid;    /* Owner's user ID */
    gid_t    gid;    /* Owner's group ID */
    uid_t    cuid;   /* Creator's user ID */
    gid_t    cgid;   /* Creator's group ID */
    mode_t   mode;   /* Read/write permission */
    unsigned short seq;     /* Sequence number */
    key_t    key;    /* Key */
};

/* Mode bits for IPC objects */
#define IPC_CREAT   0001000     /* Create if key is nonexistent */
#define IPC_EXCL    0002000     /* Fail if key exists */
#define IPC_NOWAIT  0004000     /* Return error on wait */

/* Keys */
#define IPC_PRIVATE ((key_t)0)  /* Private key */

/* Control commands used with semctl(), msgctl(), and shmctl() */
#define IPC_RMID    0           /* Remove identifier */
#define IPC_SET     1           /* Set ipc_perm options */
#define IPC_STAT    2           /* Get ipc_perm options */
#define IPC_INFO    3           /* Get info about system limits */

/* Permission flag definitions */
#define IPC_R       0000400     /* Read permission */
#define IPC_W       0000200     /* Write permission */
#define IPC_M       0010000     /* Modify permission */

/* Special key values */
#define IPC_PRIVATE_KEY  0      /* Private key value */

/* Function prototypes */
key_t ftok(const char *pathname, int proj_id);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_IPC_H */