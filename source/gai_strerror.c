#include <netdb.h>

const char *gai_strerror(int errcode) {
    switch (errcode) {
        case EAI_AGAIN:
            return "Temporary failure in name resolution";
        case EAI_BADFLAGS:
            return "Invalid value for ai_flags";
        case EAI_FAIL:
            return "Non-recoverable failure in name resolution";
        case EAI_FAMILY:
            return "ai_family not supported";
        case EAI_MEMORY:
            return "Memory allocation failure";
        case EAI_NONAME:
            return "Name or service not known";
        case EAI_SERVICE:
            return "Service name not supported for ai_socktype";
        case EAI_SOCKTYPE:
            return "ai_socktype not supported";
        case EAI_SYSTEM:
            return "System error returned in errno";
        case EAI_OVERFLOW:
            return "Argument buffer overflow";
        default:
            return "Unknown error";
    }
}
