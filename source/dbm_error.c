#include <ndbm.h>

int dbm_error(DBM *db) {
    if (!db) {
        return 1;
    }
    
    return db->error;
}