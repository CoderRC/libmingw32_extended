#include <netdb.h>
#include <string.h>

struct servent *getservbyname(const char *name, const char *proto) {
    if (!name) return NULL;
    
    setservent(1);
    struct servent *se;
    
    while ((se = getservent()) != NULL) {
        if (strcmp(se->s_name, name) == 0) {
            if (!proto || strcmp(se->s_proto, proto) == 0) {
                return se;
            }
        }
        
        if (se->s_aliases) {
            for (int i = 0; se->s_aliases[i]; i++) {
                if (strcmp(se->s_aliases[i], name) == 0) {
                    if (!proto || strcmp(se->s_proto, proto) == 0) {
                        return se;
                    }
                }
            }
        }
    }
    
    return NULL;
}