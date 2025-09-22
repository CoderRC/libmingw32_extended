#include <netdb.h>
#include <stdio.h>

extern FILE *proto_file;

void endprotoent(void) {
    if (proto_file) {
        fclose(proto_file);
        proto_file = NULL;
    }
}