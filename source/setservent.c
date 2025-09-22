#include <netdb.h>
#include <stdio.h>

extern FILE *serv_file;

void setservent(int stayopen) {
    if (serv_file) {
        rewind(serv_file);
    }
}