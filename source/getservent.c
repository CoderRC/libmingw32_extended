#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *serv_file = NULL;
static struct servent serv_entry;
static char serv_line[1024];
static char *serv_aliases[32];

struct servent *getservent(void) {
if (!serv_file) {
        serv_file = fopen("C:\\Windows\\System32\\drivers\\etc\\services", "r");
        if (!serv_file) {
            serv_file = fopen("/etc/services", "r");
        }
        if (!serv_file) return NULL;
    }
    
    while (fgets(serv_line, sizeof(serv_line), serv_file)) {
        char *line = serv_line;
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '#' || *line == '\n' || *line == '\0') continue;
        
        char *token = strtok(line, " \t\n");
        if (!token) continue;
        
        serv_entry.s_name = token;
        
        int alias_count = 0;
        while ((token = strtok(NULL, " \t\n")) && strchr(token, '/') == NULL) {
            if (alias_count < 31) {
                serv_aliases[alias_count++] = token;
            }
        }
        serv_aliases[alias_count] = NULL;
        serv_entry.s_aliases = serv_aliases;
        
        if (!token) continue;
        
        char *slash = strchr(token, '/');
        if (!slash) continue;
        
*slash = '\0';
        serv_entry.s_port = htons(atoi(token));
        serv_entry.s_proto = slash + 1;
        
        // Remove any trailing whitespace from protocol
        char *end = serv_entry.s_proto + strlen(serv_entry.s_proto) - 1;
        while (end > serv_entry.s_proto && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
            *end = '\0';
            end--;
        }
        
        return &serv_entry;
    }
    
    return NULL;
}