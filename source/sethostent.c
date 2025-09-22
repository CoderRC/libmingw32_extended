#include <netdb.h>
#include <stdio.h>

extern FILE *host_file;

void sethostent(int stayopen) {
    if (host_file) {
        rewind(host_file);
    }
}