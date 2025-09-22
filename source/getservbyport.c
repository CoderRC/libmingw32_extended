#include <netdb.h>
#include <string.h>

struct servent *getservbyport(int port, const char *proto) {
    setservent(1);
    struct servent *se;
    
while ((se = getservent()) != NULL) {
        if (se->s_port == htons(port)) {
            if (!proto || strcmp(se->s_proto, proto) == 0) {
                return se;
            }
        }
    }
    
    return NULL;
}