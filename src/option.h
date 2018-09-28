#ifndef OPTION_H
#define OPTION_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#ifndef BUG_REPORT
#include <stdio.h>
#define BUG_REPORT() (fprintf(stderr, "error: there is a bug! (%s, %s, %d)", __FILE__, __FUNCTION__, __LINE__))
#endif

typedef struct _sFLAG_PROPERY{
    char *short_form;
    char *long_form;
    int  (*contents_checker)(char **contents, int content_num);
    int  content_num_min;
    int  content_num_max;
    int  appeared_yet;
}sFLAG_PROPERY;

typedef struct _sFLAG_PROPERY_DB{
    int            entry_num;
    sFLAG_PROPERY *entries;
}sFLAG_PROPERY_DB;

extern sFLAG_PROPERY_DB
*genFlagPropDB(
        int entry_num);

extern int
add2FlagPropDB(
        sFLAG_PROPERY_DB *db,
        char             *short_form,
        char             *long_form,
        int              content_num_min,
        int              content_num_max,
        int             (*contents_checker)(char **contents, int content_num));

extern void
freeFlagPropDB(
        sFLAG_PROPERY_DB *db);

/* ========================================= */

/* related functions are optionDecorder and freeOptionInfo */
typedef struct _sOPT_GROUP{
    char  *flag;          /* string begins with - or --, assigned label */
    int    content_num;   /* element count of content */
    char **contents;      /* strings not beginning with - or -- are assigned conent */
    int    err_code;     
}sOPT_GROUP;

typedef struct _sOPT_GROUP_DB{
    int         opt_grp_num;
    sOPT_GROUP *opt_grps;
    int         content_num;
    char      **contents;
}sOPT_GROUP_DB;

extern int
groupingOpt( /* passing the number of arguments and arguments at program execution breaks it into sOPTION_INFO */
        int               argc,            /* [in] assume argumrnt of main */
        char            **argv,            /* [in] assume argument of main */
        sFLAG_PROPERY_DB *flag_prop_db,
        sOPT_GROUP_DB   **opt_grp_db);

extern void
freeOptGroupDB(
        sOPT_GROUP_DB *opt_group_db);

#endif
