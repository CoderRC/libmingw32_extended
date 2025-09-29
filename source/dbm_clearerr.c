#include <ndbm.h>

int dbm_clearerr(DBM *db) {
    if (!db) {
        return -1;
    }
    
    db->error = 0;
    return 0;
}