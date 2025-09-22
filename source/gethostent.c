#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

FILE *host_file = NULL;
static struct hostent host_entry;
static char host_line[1024];
static char *host_aliases[32];
static char *host_addr_list[2];
static struct in_addr host_addr;

struct hostent *gethostent(void) {
if (!host_file) {
        host_file = fopen("C:\\Windows\\System32\\drivers\\etc\\hosts", "r");
        if (!host_file) {
            host_file = fopen("/etc/hosts", "r");
        }
        if (!host_file) return NULL;
    }
    
    while (fgets(host_line, sizeof(host_line), host_file)) {
        char *line = host_line;
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '#' || *line == '\n' || *line == '\0') continue;
        
        char *ip_token = strtok(line, " \t\n");
        if (!ip_token) continue;
        
        host_addr.s_addr = inet_addr(ip_token);
        if (host_addr.s_addr == INADDR_NONE) continue;
        
        char *name_token = strtok(NULL, " \t\n");
        if (!name_token) continue;
        
        host_entry.h_name = name_token;
        
        int alias_count = 0;
        char *alias_token;
        while ((alias_token = strtok(NULL, " \t\n")) && alias_count < 31) {
            host_aliases[alias_count++] = alias_token;
        }
        host_aliases[alias_count] = NULL;
        host_entry.h_aliases = host_aliases;
        
        host_entry.h_addrtype = AF_INET;
        host_entry.h_length = sizeof(struct in_addr);
        host_addr_list[0] = (char*)&host_addr;
        host_addr_list[1] = NULL;
        host_entry.h_addr_list = host_addr_list;
        
        return &host_entry;
    }
    
    return NULL;
}