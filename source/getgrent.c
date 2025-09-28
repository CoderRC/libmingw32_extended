#include <grp.h>

static int group_index = 0;

struct group *getgrent(void) {
    static struct group grp;
    static char name[] = "users";
    static char *members[] = {NULL};
    
    if (group_index > 0) return NULL;
    
    grp.gr_name = name;
    grp.gr_gid = 1000;
    grp.gr_mem = members;
    
    group_index++;
    return &grp;
}

void setgrent(void) {
    group_index = 0;
}

void endgrent(void) {
    group_index = 0;
}