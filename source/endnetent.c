#include <netdb.h>
#include <stdio.h>

extern FILE *net_file;

void endnetent(void) {
    if (net_file) {
        fclose(net_file);
        net_file = NULL;
    }
}