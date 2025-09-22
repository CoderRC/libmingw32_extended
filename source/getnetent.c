#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>

FILE *net_file = NULL;
static struct netent net_entry;
static char net_line[1024];
static char *net_aliases[32];

struct netent *getnetent(void) {
if (!net_file) {
        net_file = fopen("C:\\Windows\\System32\\drivers\\etc\\networks", "r");
        if (!net_file) {
            net_file = fopen("/etc/networks", "r");
        }
        if (!net_file) return NULL;
    }
    
    while (fgets(net_line, sizeof(net_line), net_file)) {
        char *line = net_line;
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '#' || *line == '\n' || *line == '\0') continue;
        
        char *token = strtok(line, " \t\n");
        if (!token) continue;
        
        net_entry.n_name = token;
        
        int alias_count = 0;
        char *next_token;
        while ((next_token = strtok(NULL, " \t\n")) && !isdigit(next_token[0]) && strchr(next_token, '.') == NULL) {
            if (alias_count < 31) {
                net_aliases[alias_count++] = next_token;
            }
        }
        net_aliases[alias_count] = NULL;
        net_entry.n_aliases = net_aliases;
        
        if (next_token) {
            net_entry.n_net = inet_addr(next_token);
        } else {
            token = strtok(NULL, " \t\n");
            if (!token) continue;
            net_entry.n_net = inet_addr(token);
        }
        
        net_entry.n_addrtype = AF_INET;
        
        return &net_entry;
    }
    
    return NULL;
}