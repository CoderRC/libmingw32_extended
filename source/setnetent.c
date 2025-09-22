#include <netdb.h>
#include <stdio.h>

extern FILE *net_file;

void setnetent(int stayopen) {
    if (net_file) {
        rewind(net_file);
    }
}