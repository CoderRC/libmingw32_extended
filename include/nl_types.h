#ifndef _NL_TYPES_H
#define _NL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Message catalog descriptor type */
typedef void *nl_catd;

/* Message catalog item type */
typedef int nl_item;

/* Constants for catopen() */
#define NL_SETD         1   /* Set default domain */
#define NL_CAT_LOCALE   1   /* Use locale for message catalog */

/* Error return values */
#define CATD_ERR        ((nl_catd)-1)

/* Message set and message number limits */
#define NL_SETMAX       255     /* Maximum set number */
#define NL_MSGMAX       32767   /* Maximum message number */
#define NL_TEXTMAX      2048    /* Maximum message text length */

/* Special set numbers */
#define NL_SETD_DEFAULT 0       /* Default set */

/* Message catalog functions */
int       catclose(nl_catd catd);
char     *catgets(nl_catd catd, int set_id, int msg_id, const char *s);
nl_catd   catopen(const char *name, int oflag);

#ifdef __cplusplus
}
#endif

#endif /* _NL_TYPES_H */