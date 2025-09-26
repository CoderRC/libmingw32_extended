#ifndef _FMTMSG_H
#define _FMTMSG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Source classification */
#define MM_HARD   0x001L
#define MM_SOFT   0x002L
#define MM_FIRM   0x004L

/* Source identification */
#define MM_APPL   0x008L
#define MM_UTIL   0x010L
#define MM_OPSYS  0x020L

/* Severity classification */
#define MM_RECOVER 0x040L
#define MM_NRECOV  0x080L

/* Action classification */
#define MM_HALT    0x100L
#define MM_ERROR   0x200L
#define MM_WARNING 0x400L
#define MM_INFO    0x800L

/* No severity */
#define MM_NOSEV   0

/* Display classification */
#define MM_PRINT   0x1000L
#define MM_CONSOLE 0x2000L

/* Null values for fmtmsg() arguments */
#define MM_NULLLBL ((char*)0)
#define MM_NULLSEV 0
#define MM_NULLMC  0L
#define MM_NULLTXT ((char*)0)
#define MM_NULLACT ((char*)0)
#define MM_NULLTAG ((char*)0)

/* Return values */
#define MM_OK     0
#define MM_NOTOK  -1
#define MM_NOMSG  1
#define MM_NOCON  2

/* Function prototype */
int fmtmsg(long classification, const char *label, int severity,
           const char *text, const char *action, const char *tag);

#ifdef __cplusplus
}
#endif

#endif /* _FMTMSG_H */