#ifndef OPTION_H
#define OPTION_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

#ifndef BUG_REPORT
#include <stdio.h>
#define BUG_REPORT() (fprintf(stderr, "error: there is a bug! (%s, %s, %d)", __FILE__, __FUNCTION__, __LINE__))
#endif

typedef enum{
    OPTION_SUCCESS             = 0,
    OPTION_OPT_NAME_IS_NULL    = 1,
    OPTION_OUT_OF_MEMORY       = 2,
    OPTION_MIN_BIGGER_THAN_MAX = 3,
    OPTION_OPT_PROP_DB_IS_NULL = 4,
    OPTION_DUPLICATE_SAME_OPT  = 5,
    OPTION_TOO_MANY_CONTENTS   = 6,
    OPTION_TOO_LITTLE_CONTENTS = 7,
}option_errcode_t;

/* プログラムで使用できるオプションの情報を保持する構造体 */
typedef struct _opt_property_t{
    char *short_form;                                           /* オプションの短縮形式. 例えば"-v" */
    char *long_form;                                            /* オプションの詳細形式. 例えば"--version" */
    int  (*contents_checker)(char **contents, int content_num); /* オプションに付属するcontentsの正しさを調べるコールバック関数 */
    int  content_num_min;                                       /* オプションに付属するcontentsの最小数 */
    int  content_num_max;                                       /* オプションに付属するcontentsの最大数 */
    int  appeared_yet;                                          /* 同じオプションがすでに指定されたかチェックするためのメモとして用いる */
}opt_property_t;

/* opt_property_tのエントリを保持するための構造体 */
typedef struct _opt_property_db_t{
    int             prop_num; /* propsのサイズ */
    opt_property_t *props;    /* opt_property_tの配列 */
}opt_property_db_t;

/* プログラム実行時に指定した各オプションの情報を保持するための構造体 */
typedef struct _opt_group_t{
    char  *option;        /* 例えば "gcc -Wall -O3 -I ./hoge ./geho -o ./foo bar1.c bar2.c" の -Wall, -O3, -I, -o がそれぞれ別のopt_group_tのoptionに割り当てられる */
    int    content_num;   /* 例えば option が -I なら2になる */
    char **contents;      /* 例えば option が -I なら ./hoge, ./geho になる */
    int    err_code;      /* 対応するopt_property_tに登録されているcontents_checker関数の引数にcontent_numとcontentsを適用した際の戻り値  */
}opt_group_t;

/* opt_group_tのエントリを保持するための構造体 */
typedef struct _opt_group_db_t{
    int          grp_num;    /* opt_group_tの配列のサイズ */
    opt_group_t *grps;       /* opt_group_tの配列 */
    int         optless_num; /* 対応するオプションが無いコンテンツの数 */
    char      **optless;     /* 対応するオプションが無いコンテンツ. 例えば, gcc -o hoge hoge.c geho.c のhoge.cとgeho.c */
}opt_group_db_t;

extern opt_property_db_t* /* 生成されたopt_property_db_tのメモリ領域のポインタ */
genOptPropDB(     
        int prop_num); /* 登録するopt_property_tの数 */

extern int /* option_errcode_tのどれか */
regOptProp( /* opt_property_db_tのエントリを追加する関数 */
        opt_property_db_t  *db,             /* [out] 登録先(genOptPropDBで作成したopt_property_db_t) */
        char               *short_form,     /* [in] オプションの短縮形式 */
        char               *long_form,      /* [in] オプションの詳細形式 */
        int                content_num_min, /* オプションに付属するコンテンツの最少数 */
        int                content_num_max, /* オプションに付属するコンテンツの最大数 */
        int              (*contents_checker)(char **contents, int content_num)); /* オプションのコンテンツをチェックするコールバック関数 */

extern void
freeOptPropDB( /* opt_property_db_tのメンバのメモリ領域を再帰的に開放する関数 */
        opt_property_db_t *db); /* [in] 開放するopt_property_db_t */

extern int /* option_errcode_tのどれか */
groupingOpt( /* オプション情報が登録されたopt_property_dbをもとにmainの引数で取得したargcとargvをグループに分類してopt_group_db_tのエントリに登録する関数 */
        opt_property_db_t *opt_prop_db,  /* [in] オプション情報が登録されたopt_property_db_t */
        int                argc,         /* mainの引数で受け取ったプログラムの引数の数(プログラム名含む) */
        char             **argv,         /* [in] mainの引数で受け取ったプログラムの引数(プログラム名含む) */
        opt_group_db_t   **opt_grp_db);  /* [out] グルーピングされたオプション情報 */

extern void
freeOptGroupDB( /* opt_group_db_tのメンバのメモリ領域を再帰的に解放 */
        opt_group_db_t *opt_group_db); /* [in] 開放するopt_group_db_t */

#endif
