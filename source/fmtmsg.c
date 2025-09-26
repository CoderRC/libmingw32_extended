#include <fmtmsg.h>
#include <stdio.h>
#include <string.h>

int fmtmsg(long classification, const char *label, int severity,
           const char *text, const char *action, const char *tag) {
    
    int result = MM_OK;
    FILE *output = NULL;
    
    /* Determine output destination */
    if (classification & MM_CONSOLE) {
        output = stderr; /* Use stderr as console on Windows */
    } else if (classification & MM_PRINT) {
        output = stderr;
    } else {
        output = stderr; /* Default to stderr */
    }
    
    if (!output) {
        return MM_NOTOK;
    }
    
    /* Format and print the message */
    if (label && *label) {
        fprintf(output, "%s: ", label);
    }
    
    /* Print severity */
    switch (severity) {
        case MM_HALT:
            fprintf(output, "HALT: ");
            break;
        case MM_ERROR:
            fprintf(output, "ERROR: ");
            break;
        case MM_WARNING:
            fprintf(output, "WARNING: ");
            break;
        case MM_INFO:
            fprintf(output, "INFO: ");
            break;
        default:
            break;
    }
    
    /* Print text */
    if (text && *text) {
        fprintf(output, "%s", text);
    }
    
    /* Print action */
    if (action && *action) {
        fprintf(output, "\nTO FIX: %s", action);
    }
    
    /* Print tag */
    if (tag && *tag) {
        fprintf(output, " %s", tag);
    }
    
    fprintf(output, "\n");
    fflush(output);
    
    return result;
}