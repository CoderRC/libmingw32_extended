#ifndef _MONETARY_H
#define _MONETARY_H

#include <stddef.h>
#include <sys/types.h>
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LOCALE_T_DEFINED
typedef void *locale_t;
#define _LOCALE_T_DEFINED
#endif

#ifndef _SSIZE_T_DEFINED
typedef long ssize_t;
#define _SSIZE_T_DEFINED
#endif

/* Monetary formatting constants */
#define MON_DECIMAL_POINT    0
#define MON_THOUSANDS_SEP    1
#define MON_GROUPING         2
#define MON_INT_CURR_SYMBOL  3
#define MON_CURRENCY_SYMBOL  4
#define MON_MON_DECIMAL_POINT 5
#define MON_MON_THOUSANDS_SEP 6
#define MON_MON_GROUPING     7
#define MON_POSITIVE_SIGN    8
#define MON_NEGATIVE_SIGN    9
#define MON_INT_FRAC_DIGITS  10
#define MON_FRAC_DIGITS      11
#define MON_P_CS_PRECEDES    12
#define MON_P_SEP_BY_SPACE   13
#define MON_N_CS_PRECEDES    14
#define MON_N_SEP_BY_SPACE   15
#define MON_P_SIGN_POSN      16
#define MON_N_SIGN_POSN      17
#define MON_INT_P_CS_PRECEDES 18
#define MON_INT_P_SEP_BY_SPACE 19
#define MON_INT_N_CS_PRECEDES 20
#define MON_INT_N_SEP_BY_SPACE 21
#define MON_INT_P_SIGN_POSN  22
#define MON_INT_N_SIGN_POSN  23

/* struct lconv is defined in locale.h */

/* Function prototypes */
ssize_t strfmon(char *restrict s, size_t maxsize, const char *restrict format, ...);
ssize_t strfmon_l(char *restrict s, size_t maxsize, locale_t loc, const char *restrict format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _MONETARY_H */