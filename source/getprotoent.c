#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE *proto_file = NULL;
static struct protoent proto_entry;
static char proto_line[1024];
static char *proto_aliases[32];

struct protoent *getprotoent(void) {
if (!proto_file) {
        proto_file = fopen("C:\\Windows\\System32\\drivers\\etc\\protocol", "r");
        if (!proto_file) {
            proto_file = fopen("/etc/protocols", "r");
        }
        if (!proto_file) return NULL;
    }
    
    while (fgets(proto_line, sizeof(proto_line), proto_file)) {
        char *line = proto_line;
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '#' || *line == '\n' || *line == '\0') continue;
        
        char *token = strtok(line, " \t\n");
        if (!token) continue;
        
        proto_entry.p_name = token;
        
        int alias_count = 0;
        char *next_token;
        while ((next_token = strtok(NULL, " \t\n")) && !isdigit(next_token[0])) {
            if (alias_count < 31) {
                proto_aliases[alias_count++] = next_token;
            }
        }
        proto_aliases[alias_count] = NULL;
        proto_entry.p_aliases = proto_aliases;
        
        if (next_token) {
            proto_entry.p_proto = atoi(next_token);
        } else {
            token = strtok(NULL, " \t\n");
            if (!token) continue;
            proto_entry.p_proto = atoi(token);
        }
        
        return &proto_entry;
    }
    
    return NULL;
}