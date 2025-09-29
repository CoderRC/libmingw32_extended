#ifndef _NDBM_H
#define _NDBM_H

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* mode_t is defined in sys/types.h */

/* Data item structure */
typedef struct {
    void   *dptr;   /* A pointer to the application's data */
    size_t  dsize;  /* The size of the object pointed to by dptr */
} datum;

/* Database management structure */
struct DBM {
    int fd_pag;     /* Page file descriptor */
    int fd_dir;     /* Directory file descriptor */
    int error;      /* Error flag */
    int rdonly;     /* Read-only flag */
    char *filename; /* Database filename */
};
typedef struct DBM DBM;

/* Store mode constants */
#define DBM_INSERT  0   /* Insert new entry only */
#define DBM_REPLACE 1   /* Replace existing entry */

/* File access modes (from fcntl.h) */
#ifndef O_RDONLY
#define O_RDONLY    0x0000  /* Read only */
#define O_WRONLY    0x0001  /* Write only */
#define O_RDWR      0x0002  /* Read/write */
#define O_CREAT     0x0100  /* Create if nonexistent */
#define O_EXCL      0x0200  /* Exclusive use flag */
#define O_TRUNC     0x0400  /* Truncate flag */
#endif

/* Database limits */
#define DBM_SUFFIX_LEN  4       /* Length of database file suffix */
#define DBM_PAGSIZE     1024    /* Database page size */
#define DBM_MAXKEY      1023    /* Maximum key size */
#define DBM_MAXDATA     1023    /* Maximum data size */

/* Error codes */
#define DBM_SUCCESS     0       /* Success */
#define DBM_ERROR       -1      /* General error */
#define DBM_NOMEM       -2      /* Out of memory */
#define DBM_NOFILE      -3      /* File not found */
#define DBM_BADFILE     -4      /* Bad file format */
#define DBM_LOCKED      -5      /* Database locked */
#define DBM_READONLY    -6      /* Read-only database */

/* Null datum constant */
#define DBM_NULL_DATUM  {NULL, 0}

/* Function prototypes */
int     dbm_clearerr(DBM *db);
void    dbm_close(DBM *db);
int     dbm_delete(DBM *db, datum key);
int     dbm_error(DBM *db);
datum   dbm_fetch(DBM *db, datum key);
datum   dbm_firstkey(DBM *db);
datum   dbm_nextkey(DBM *db);
DBM    *dbm_open(const char *file, int open_flags, mode_t file_mode);
int     dbm_store(DBM *db, datum key, datum content, int store_mode);

/* Additional utility functions */
int     dbm_rdonly(DBM *db);        /* Check if database is read-only */
int     dbm_pagfno(DBM *db);        /* Get page file descriptor */
int     dbm_dirfno(DBM *db);        /* Get directory file descriptor */

#ifdef __cplusplus
}
#endif

#endif /* _NDBM_H */