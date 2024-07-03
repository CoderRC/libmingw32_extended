/* source/gai_strerror.c */

#include <netdb.h>

const char *
gai_strerror(int errcode)
{
    switch (errcode) {
        case EAI_SUCCESS:
            return "Success";
        case EAI_FAIL:
            return "Non-recoverable failure in name resolution";
        case EAI_FAMILY:
            return "ai_family not supported";
        case EAI_MEMORY:
            return "Memory allocation failure";
        case EAI_NONAME:
            return "Name or service not known";
        case EAI_SERVICE:
            return "Servname not supported for ai_socktype";
        case EAI_SOCKTYPE:
            return "ai_socktype not supported";
        default:
            return "Unknown error";
    }
}