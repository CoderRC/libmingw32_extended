#include <netdb.h>

struct netent *getnetbyaddr(uint32_t net, int type) {
    if (type != AF_INET) return NULL;
    
    setnetent(1);
    struct netent *ne;
    
    while ((ne = getnetent()) != NULL) {
        if (ne->n_net == net && ne->n_addrtype == type) {
            return ne;
        }
    }
    
    return NULL;
}