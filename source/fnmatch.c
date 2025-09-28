#include <fnmatch.h>
#include <ctype.h>

static int match_char(char p, char s, int flags) {
    if (flags & FNM_CASEFOLD) {
        return tolower(p) == tolower(s);
    }
    return p == s;
}

static int fnmatch_internal(const char *pattern, const char *string, int flags) {
    const char *p = pattern;
    const char *s = string;
    
    while (*p) {
        switch (*p) {
            case '*':
                p++;
                if (!*p) return 0; /* Pattern ends with *, matches rest */
                
                while (*s) {
                    if (fnmatch_internal(p, s, flags) == 0) return 0;
                    if ((flags & FNM_PATHNAME) && *s == '/') break;
                    s++;
                }
                return FNM_NOMATCH;
                
            case '?':
                if (!*s) return FNM_NOMATCH;
                if ((flags & FNM_PATHNAME) && *s == '/') return FNM_NOMATCH;
                if ((flags & FNM_PERIOD) && *s == '.' && 
                    (s == string || ((flags & FNM_PATHNAME) && s[-1] == '/'))) {
                    return FNM_NOMATCH;
                }
                p++;
                s++;
                break;
                
            case '[':
                if (!*s) return FNM_NOMATCH;
                if ((flags & FNM_PATHNAME) && *s == '/') return FNM_NOMATCH;
                if ((flags & FNM_PERIOD) && *s == '.' && 
                    (s == string || ((flags & FNM_PATHNAME) && s[-1] == '/'))) {
                    return FNM_NOMATCH;
                }
                
                p++;
                int negate = (*p == '!' || *p == '^');
                if (negate) p++;
                
                int matched = 0;
                while (*p && *p != ']') {
                    if (p[1] == '-' && p[2] && p[2] != ']') {
                        if ((flags & FNM_CASEFOLD) ? 
                            (tolower(*s) >= tolower(*p) && tolower(*s) <= tolower(p[2])) :
                            (*s >= *p && *s <= p[2])) {
                            matched = 1;
                        }
                        p += 3;
                    } else {
                        if (match_char(*p, *s, flags)) matched = 1;
                        p++;
                    }
                }
                
                if (!*p) return FNM_NOMATCH; /* Unclosed bracket */
                if (matched == negate) return FNM_NOMATCH;
                p++;
                s++;
                break;
                
            case '\\':
                if (!(flags & FNM_NOESCAPE) && p[1]) {
                    p++;
                }
                /* Fall through */
                
            default:
                if (!*s) return FNM_NOMATCH;
                if ((flags & FNM_PERIOD) && *s == '.' && 
                    (s == string || ((flags & FNM_PATHNAME) && s[-1] == '/'))) {
                    if (*p != '.') return FNM_NOMATCH;
                }
                if (!match_char(*p, *s, flags)) return FNM_NOMATCH;
                p++;
                s++;
                break;
        }
    }
    
    return *s ? FNM_NOMATCH : 0;
}

int fnmatch(const char *pattern, const char *string, int flags) {
    if (!pattern || !string) return FNM_NOMATCH;
    return fnmatch_internal(pattern, string, flags);
}