#ifndef _GLOB_H
#define _GLOB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* glob_t structure */
typedef struct {
    size_t   gl_pathc;  /* Count of paths matched by pattern */
    char   **gl_pathv;  /* Pointer to a list of matched pathnames */
    size_t   gl_offs;   /* Slots to reserve at the beginning of gl_pathv */
} glob_t;

/* Flags for glob() */
#define GLOB_APPEND   0x01
#define GLOB_DOOFFS   0x02
#define GLOB_ERR      0x04
#define GLOB_MARK     0x08
#define GLOB_NOCHECK  0x10
#define GLOB_NOESCAPE 0x20
#define GLOB_NOSORT   0x40

/* Error return values */
#define GLOB_ABORTED  1
#define GLOB_NOMATCH  2
#define GLOB_NOSPACE  3

/* Function prototypes */
int glob(const char *restrict pattern, int flags, 
         int (*errfunc)(const char *, int), glob_t *restrict pglob);
void globfree(glob_t *pglob);

#ifdef __cplusplus
}
#endif

#endif /* _GLOB_H */