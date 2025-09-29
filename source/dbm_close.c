#include <ndbm.h>
#include <stdlib.h>

void dbm_close(DBM *db) {
    if (db) {
        if (db->filename) {
            free(db->filename);
        }
        free(db);
    }
}