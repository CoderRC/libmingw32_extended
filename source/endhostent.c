#include <netdb.h>
#include <stdio.h>

extern FILE *host_file;

void endhostent(void) {
    if (host_file) {
        fclose(host_file);
        host_file = NULL;
    }
}