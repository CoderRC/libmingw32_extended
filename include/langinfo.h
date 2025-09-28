#ifndef _LANGINFO_H
#define _LANGINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LOCALE_T_DEFINED
typedef void *locale_t;
#define _LOCALE_T_DEFINED
#endif

typedef int nl_item;

/* LC_CTYPE */
#define CODESET     1

/* LC_TIME */
#define D_T_FMT     2
#define D_FMT       3
#define T_FMT       4
#define T_FMT_AMPM  5
#define AM_STR      6
#define PM_STR      7

#define DAY_1       8
#define DAY_2       9
#define DAY_3       10
#define DAY_4       11
#define DAY_5       12
#define DAY_6       13
#define DAY_7       14

#define ABDAY_1     15
#define ABDAY_2     16
#define ABDAY_3     17
#define ABDAY_4     18
#define ABDAY_5     19
#define ABDAY_6     20
#define ABDAY_7     21

#define MON_1       22
#define MON_2       23
#define MON_3       24
#define MON_4       25
#define MON_5       26
#define MON_6       27
#define MON_7       28
#define MON_8       29
#define MON_9       30
#define MON_10      31
#define MON_11      32
#define MON_12      33

#define ABMON_1     34
#define ABMON_2     35
#define ABMON_3     36
#define ABMON_4     37
#define ABMON_5     38
#define ABMON_6     39
#define ABMON_7     40
#define ABMON_8     41
#define ABMON_9     42
#define ABMON_10    43
#define ABMON_11    44
#define ABMON_12    45

#define ALTMON_1    46
#define ALTMON_2    47
#define ALTMON_3    48
#define ALTMON_4    49
#define ALTMON_5    50
#define ALTMON_6    51
#define ALTMON_7    52
#define ALTMON_8    53
#define ALTMON_9    54
#define ALTMON_10   55
#define ALTMON_11   56
#define ALTMON_12   57

#define ABALTMON_1  58
#define ABALTMON_2  59
#define ABALTMON_3  60
#define ABALTMON_4  61
#define ABALTMON_5  62
#define ABALTMON_6  63
#define ABALTMON_7  64
#define ABALTMON_8  65
#define ABALTMON_9  66
#define ABALTMON_10 67
#define ABALTMON_11 68
#define ABALTMON_12 69

#define ERA         70
#define ERA_D_FMT   71
#define ERA_D_T_FMT 72
#define ERA_T_FMT   73
#define ALT_DIGITS  74

/* LC_NUMERIC */
#define RADIXCHAR   75
#define THOUSEP     76

/* LC_MESSAGES */
#define YESEXPR     77
#define NOEXPR      78

/* LC_MONETARY */
#define CRNCYSTR    79

char *nl_langinfo(nl_item);
char *nl_langinfo_l(nl_item, locale_t);

#ifdef __cplusplus
}
#endif

#endif /* _LANGINFO_H */