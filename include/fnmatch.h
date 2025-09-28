#ifndef _FNMATCH_H
#define _FNMATCH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Return value */
#define FNM_NOMATCH 1

/* Flags */
#define FNM_PATHNAME  0x01
#define FNM_PERIOD    0x02
#define FNM_NOESCAPE  0x04
#define FNM_CASEFOLD  0x08
#define FNM_IGNORECASE FNM_CASEFOLD

/* Function prototype */
int fnmatch(const char *pattern, const char *string, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _FNMATCH_H */