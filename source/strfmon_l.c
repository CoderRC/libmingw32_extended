#include <monetary.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef struct {
    char *name;
    int category;
} locale_data_t;

static const char* get_currency_symbol(const char *locale_name) {
    if (!locale_name) return "$";
    
    if (strncmp(locale_name, "en", 2) == 0) return "$";
    if (strncmp(locale_name, "zh", 2) == 0) return "¥";
    if (strncmp(locale_name, "ja", 2) == 0) return "¥";
    if (strncmp(locale_name, "es", 2) == 0) return "€";
    if (strncmp(locale_name, "fr", 2) == 0) return "€";
    if (strncmp(locale_name, "de", 2) == 0) return "€";
    if (strncmp(locale_name, "pt", 2) == 0) return "R$";
    if (strncmp(locale_name, "ru", 2) == 0) return "₽";
    if (strncmp(locale_name, "hi", 2) == 0) return "₹";
    if (strncmp(locale_name, "bn", 2) == 0) return "৳";
    
    return "$";
}

ssize_t strfmon_l(char *restrict s, size_t maxsize, locale_t loc, const char *restrict format, ...) {
    va_list args;
    va_start(args, format);
    
    if (!s || !format || maxsize == 0) {
        va_end(args);
        return -1;
    }
    
    const char *currency = "$";
    if (loc) {
        locale_data_t *locale_data = (locale_data_t *)loc;
        currency = get_currency_symbol(locale_data->name);
    }
    
    const char *p = format;
    char *dest = s;
    size_t remaining = maxsize - 1;
    
    while (*p && remaining > 0) {
        if (*p == '%') {
            p++;
            if (*p == 'n') {
                // National currency format
                double value = va_arg(args, double);
                int written = snprintf(dest, remaining, "%s%.2f", currency, value);
                if (written > 0 && (size_t)written < remaining) {
                    dest += written;
                    remaining -= written;
                }
            } else if (*p == 'i') {
                // International currency format
                double value = va_arg(args, double);
                int written = snprintf(dest, remaining, "%s %.2f", currency, value);
                if (written > 0 && (size_t)written < remaining) {
                    dest += written;
                    remaining -= written;
                }
            } else {
                if (remaining > 0) {
                    *dest++ = *p;
                    remaining--;
                }
            }
            p++;
        } else {
            *dest++ = *p++;
            remaining--;
        }
    }
    
    *dest = '\0';
    va_end(args);
    return dest - s;
}