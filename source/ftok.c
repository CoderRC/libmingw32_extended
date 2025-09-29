#include <sys/ipc.h>
#include <errno.h>
#include <string.h>

key_t ftok(const char *pathname, int proj_id) {
    if (!pathname || *pathname == '\0') {
        errno = EINVAL;
        return (key_t)-1;
    }
    
    if (proj_id == 0) {
        errno = EINVAL;
        return (key_t)-1;
    }
    
    /* Simple hash-based implementation for mingw32 */
    /* This creates a deterministic key based on pathname and project ID */
    
    key_t key = 0;
    const char *p = pathname;
    
    /* Hash the pathname */
    while (*p) {
        key = (key * 31) + *p;
        p++;
    }
    
    /* Combine with project ID */
    key = (key & 0xFFFFFF00) | (proj_id & 0xFF);
    
    /* Ensure we don't return -1 (error value) */
    if (key == (key_t)-1) {
        key = 1;
    }
    
    return key;
}