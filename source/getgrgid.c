#include <grp.h>
#include <errno.h>

struct group *getgrgid(gid_t gid) {
    static struct group grp;
    static char name[] = "users";
    static char *members[] = {NULL};
    
    grp.gr_name = name;
    grp.gr_gid = gid;
    grp.gr_mem = members;
    
    return &grp;
}