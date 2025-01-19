#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

int inet_pton(int af, const char *src, void *dst) {
    if (src == NULL || dst == NULL) {
        errno = EINVAL;
        return -1;
    }

    switch (af) {
        case AF_INET: {
            unsigned int a, b, c, d;
            if (sscanf(src, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
                return 0;
                
            if (a > 255 || b > 255 || c > 255 || d > 255)
                return 0;
                
            uint8_t *bytes = (uint8_t *)dst;
            bytes[0] = (uint8_t)a;
            bytes[1] = (uint8_t)b;
            bytes[2] = (uint8_t)c;
            bytes[3] = (uint8_t)d;
            break;
        }
        
        case AF_INET6: {
            uint16_t *words = (uint16_t *)dst;
            memset(words, 0, 16);
            int wordCount = 0;
            const char *p = src;
            char *end;
            
            // Check for compressed form
            const char *compressPos = strstr(src, "::");
            int compressLen = 0;
            
            if (compressPos != NULL) {
                // Count colons before compression
                for (const char *t = src; t < compressPos; t++)
                    if (*t == ':') wordCount++;
                    
                // Count colons after compression
                const char *t = compressPos + 2;
                while (*t) if (*t++ == ':') compressLen++;
                
                if (wordCount + compressLen >= 7) return 0;
            }
            
            // Parse address
            wordCount = 0;
            while (*p && wordCount < 8) {
                if (p == compressPos) {
                    wordCount = 8 - compressLen - 1;
                    p += 2;
                    continue;
                }
                
                words[wordCount++] = htons(strtoul(p, &end, 16));
                if (*end && *end != ':') return 0;
                p = end + 1;
            }
            
            if (wordCount != 8) return 0;
            break;
        }
        
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
    
    return 1;
}