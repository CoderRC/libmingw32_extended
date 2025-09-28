#ifndef _GRP_H
#define _GRP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _GID_T_DEFINED
typedef unsigned int gid_t;
#define _GID_T_DEFINED
#endif

struct group {
    char   *gr_name;  /* The name of the group */
    gid_t   gr_gid;   /* Numerical group ID */
    char  **gr_mem;   /* Pointer to a null-terminated array of character pointers to member names */
};

/* Function prototypes */
void           endgrent(void);
struct group  *getgrent(void);
struct group  *getgrgid(gid_t);
int            getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
struct group  *getgrnam(const char *);
int            getgrnam_r(const char *, struct group *, char *, size_t, struct group **);
void           setgrent(void);

#ifdef __cplusplus
}
#endif

#endif /* _GRP_H */