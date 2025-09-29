#include <nl_types.h>
#include <errno.h>
#include <stdlib.h>

/* Internal catalog structure */
struct catalog {
    char *name;
    int flags;
    int valid;
};

int catclose(nl_catd catd) {
    if (catd == CATD_ERR || !catd) {
        errno = EBADF;
        return -1;
    }
    
    struct catalog *cat = (struct catalog *)catd;
    
    /* Simple validity check - if the structure looks corrupted, it's likely already freed */
    if (cat->valid != 1) {
        errno = EBADF;
        return -1;
    }
    
    /* Mark as invalid to prevent double-close */
    cat->valid = 0;
    
    /* Free allocated memory */
    if (cat->name) {
        free(cat->name);
        cat->name = NULL;
    }
    
    free(cat);
    return 0;
}