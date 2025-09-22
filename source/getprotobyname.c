#include <netdb.h>
#include <string.h>

struct protoent *getprotobyname(const char *name) {
    if (!name) return NULL;
    
    setprotoent(1);
    struct protoent *pe;
    
    while ((pe = getprotoent()) != NULL) {
        if (strcmp(pe->p_name, name) == 0) {
            return pe;
        }
        
        if (pe->p_aliases) {
            for (int i = 0; pe->p_aliases[i]; i++) {
                if (strcmp(pe->p_aliases[i], name) == 0) {
                    return pe;
                }
            }
        }
    }
    
    return NULL;
}