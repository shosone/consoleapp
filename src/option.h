/* MIT License
 * 
 * Copyright (c) 2018 Sho Sone
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef OPTION_H
#define OPTION_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdbool.h>

/* プログラムで使用できるオプションの情報を保持する構造体 */
typedef struct _opt_property_t{
    char *short_form;                                           /* オプションの短縮形式. 例えば"-v" */
    char *long_form;                                            /* オプションの詳細形式. 例えば"--version" */
    int  (*contents_checker)(char **contents, int content_num); /* オプションに付属するcontentsの正しさを調べるコールバック関数 */
    int  content_num_min;                                       /* オプションに付属するcontentsの最小数 */
    int  content_num_max;                                       /* オプションに付属するcontentsの最大数 */
    bool  appeared_yet;                                          /* 同じオプションがすでに指定されたかチェックするためのメモとして用いる */
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

extern opt_group_db_t* /* option_errcode_tのどれか */
genOptGrpDB( /* オプション情報が登録されたopt_property_dbをもとにmainの引数で取得したargcとargvをグループに分類してopt_group_db_tのエントリに登録する関数 */
        opt_property_db_t *opt_prop_db,  /* [in] オプション情報が登録されたopt_property_db_t */
        int                argc,         /* mainの引数で受け取ったプログラムの引数の数(プログラム名含む) */
        char             **argv);         /* [in] mainの引数で受け取ったプログラムの引数(プログラム名含む) */

extern void
freeOptGroupDB( /* opt_group_db_tのメンバのメモリ領域を再帰的に解放 */
        opt_group_db_t *opt_group_db); /* [in] 開放するopt_group_db_t */

#endif
