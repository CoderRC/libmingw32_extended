#include <netdb.h>
#include <stdio.h>

extern FILE *proto_file;

void setprotoent(int stayopen) {
    if (proto_file) {
        rewind(proto_file);
    }
}