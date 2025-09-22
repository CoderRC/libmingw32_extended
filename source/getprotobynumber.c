#include <netdb.h>

struct protoent *getprotobynumber(int proto) {
    setprotoent(1);
    struct protoent *pe;
    
    while ((pe = getprotoent()) != NULL) {
        if (pe->p_proto == proto) {
            return pe;
        }
    }
    
    return NULL;
}