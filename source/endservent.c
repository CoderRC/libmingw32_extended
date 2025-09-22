#include <netdb.h>
#include <stdio.h>

extern FILE *serv_file;

void endservent(void) {
    if (serv_file) {
        fclose(serv_file);
        serv_file = NULL;
    }
}