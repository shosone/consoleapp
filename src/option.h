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

/* プログラムで使用できるオプションの情報を保持する構造体 */
typedef struct _sFLAG_PROPERY{
    char *short_form;                                           /* オプションの短縮形式. 例えば"-v" */
    char *long_form;                                            /* オプションの詳細形式. 例えば"--version" */
    int  (*contents_checker)(char **contents, int content_num); /* オプションに付属するcontentsの正しさを調べるコールバック関数 */
    int  content_num_min;                                       /* オプションに付属するcontentsの最小数 */
    int  content_num_max;                                       /* オプションに付属するcontentsの最大数 */
    int  appeared_yet;                                          /* 同じオプションがすでに指定されたかチェックするためのメモとして用いる */
}sFLAG_PROPERY;

/* sFLAG_PROPERYのエントリを保持するための構造体 */
typedef struct _sFLAG_PROPERY_DB{
    int            prop_num; /* sFLAG_PROPERYの配列のサイズ */
    sFLAG_PROPERY *props;    /* sFLAG_PROPERYの配列 */
}sFLAG_PROPERY_DB;

/* プログラム実行時に指定した各オプションの情報を保持するための構造体 */
typedef struct _sOPT_GROUP{
    char  *flag;          /* 例えば "gcc -Wall -O3 -I ./hoge ./geho -o ./foo bar1.c bar2.c" の -Wall, -O3, -I, -o がそれぞれ別のsOPT_GROUPのフラグに割り当てられる */
    int    content_num;   /* 例えば flag が -I なら2になる */
    char **contents;      /* 例えば flag が -I なら ./hoge, ./geho になる */
    int    err_code;      /* 対応するsFLAG_PROPERYに登録されているcontents_checker関数の引数にcontent_numとcontentsを適用した際の戻り値  */
}sOPT_GROUP;

/* sOPT_GROUPのエントリを保持するための構造体 */
typedef struct _sOPT_GROUP_DB{
    int         grp_num;      /* sOPT_GROUPの配列のサイズ */
    sOPT_GROUP *grps;         /* sOPT_GROUPの配列 */
    int         flagless_num; /* 対応するオプションが無いコンテンツの数 */
    char      **flagless;     /* 対応するオプションが無いコンテンツ. 例えば, gcc -o hoge hoge.c geho.c のhoge.cとgeho.c */
}sOPT_GROUP_DB;

extern sFLAG_PROPERY_DB* /* 生成されたsFLAG_PROPERY_DBのメモリ領域のポインタ */
genFlagPropDB(     
        int prop_num); /* 登録するsFLAG_PROPERYの数 */

extern int /* 0:success, 1:short_formがNULL, 2:out of memory, 3:content_num_minがcontent_num_maxより大きい */
addFlagProp2DB( /* sFLAG_PROPERY_DBのエントリを追加する関数 */
        sFLAG_PROPERY_DB *db,             /* [out] 登録先(genFlagPropDBで作成したsFLAG_PROPERY_DB) */
        char             *short_form,     /* [in] オプションの短縮形式 */
        char             *long_form,      /* [in] オプションの詳細形式 */
        int              content_num_min, /* オプションに付属するコンテンツの最少数 */
        int              content_num_max, /* オプションに付属するコンテンツの最大数 */
        int            (*contents_checker)(char **contents, int content_num)); /* オプションのコンテンツをチェックするコールバック関数 */

extern void
freeFlagPropDB( /* sFLAG_PROPERY_DBのメンバのメモリ領域を再帰的に開放する関数 */
        sFLAG_PROPERY_DB *db); /* [in] 開放するsFLAG_PROPERY_DB */

extern int /* 0:success, 1:out of memory, 2:flag_prop_db is null, 3:duplicate same flag, 4:too many contents, 5:too little contents */
groupingOpt( /* オプション情報が登録されたsFLAG_PROPERY_DBをもとにmainの引数で取得したargcとargvをグループに分類してsOPT_GROUP_DBのエントリに登録する関数 */
        sFLAG_PROPERY_DB *flag_prop_db, /* [in] オプション情報が登録されたsFLAG_PROPERY_DB */
        int               argc,         /* mainの引数で受け取ったプログラムの引数の数(プログラム名含む) */
        char            **argv,         /* [in] mainの引数で受け取ったプログラムの引数(プログラム名含む) */
        sOPT_GROUP_DB   **opt_grp_db);  /* [out] グルーピングされたオプション情報 */

extern void
freeOptGroupDB( /* sOPT_GROUP_DBのメンバのメモリ領域を再帰的に解放 */
        sOPT_GROUP_DB *opt_group_db); /* [in] 開放するsOPT_GROUP_DB */

#endif
