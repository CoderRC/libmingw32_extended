#include <ndbm.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

DBM *dbm_open(const char *file, int open_flags, mode_t file_mode) {
    if (!file) {
        errno = EINVAL;
        return NULL;
    }
    
    DBM *db = malloc(sizeof(DBM));
    if (!db) {
        errno = ENOMEM;
        return NULL;
    }
    
    // Initialize all fields to zero
    memset(db, 0, sizeof(DBM));
    
    db->fd_pag = 1;  /* Dummy page file descriptor */
    db->fd_dir = 2;  /* Dummy directory file descriptor */
    db->error = 0;
    db->rdonly = (open_flags & O_RDWR) ? 0 : 1;
    db->filename = malloc(strlen(file) + 1);
    if (db->filename) {
        strcpy(db->filename, file);
    }
    
    return db;
}