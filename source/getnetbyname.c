#include <netdb.h>
#include <string.h>

struct netent *getnetbyname(const char *name) {
    if (!name) return NULL;
    
    setnetent(1);
    struct netent *ne;
    
    while ((ne = getnetent()) != NULL) {
        if (strcmp(ne->n_name, name) == 0) {
            return ne;
        }
        
        if (ne->n_aliases) {
            for (int i = 0; ne->n_aliases[i]; i++) {
                if (strcmp(ne->n_aliases[i], name) == 0) {
                    return ne;
                }
            }
        }
    }
    
    return NULL;
}