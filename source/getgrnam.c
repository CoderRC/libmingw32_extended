#include <grp.h>
#include <string.h>
#include <errno.h>

struct group *getgrnam(const char *name) {
    static struct group grp;
    static char grname[] = "users";
    static char *members[] = {NULL};
    
    if (!name) {
        errno = EINVAL;
        return NULL;
    }
    
    grp.gr_name = grname;
    grp.gr_gid = 1000;
    grp.gr_mem = members;
    
    return &grp;
}