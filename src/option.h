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

#define OPTION_SUCCESS  0
#define OPTION_FAILURE -1

/* [USAGE
 * 1: startOptionAnalize()
 * 2: regOptProperty()
 * 3: groupingOpt()
 * 4: popOptionErrcode()
 * 5: popOption()
 * 6: finishOptAnalize()
 */

/* プログラム実行時に指定した各オプションの情報を保持するための構造体 */
typedef struct _opt_group_t{
    unsigned int priority;      /* popOptGroupにて取り出すオプションの順番(優先度) */
    int          content_num;   /* 例えば option が -I なら2になる */
    char       **contents;      /* 例えば option が -I なら ./hoge, ./geho になる */
}opt_group_t;

extern int /* OPTION_SUCCESS or OPTION_FAILURE */
regOptProperty( /* opt_property_db_tのエントリを追加する関数 */
        unsigned int priority,        /* popOptGroupにて取り出すオプションの順番(優先度) */
        char        *short_form,      /* [in] オプションの短縮形式 */
        char        *long_form,       /* [in] オプションの詳細形式 */
        int          content_num_min, /* オプションに付属するコンテンツの最少数 */
        int          content_num_max, /* オプションに付属するコンテンツの最大数 */
        int        (*contentsChecker)(char **contents, int content_num)); /* オプションのコンテンツをチェックするコールバック関数 */

extern int /* OPTION_SUCCESS or OPTION_FAILURE */
groupingOpt( /* cliより取得したmainの引数であるargc, argvとregOptionPropertyにて登録したオプション情報をもとにオプションをグルーピングする関数 */
        int     argc,        /* mainの第一引数 */
        char   *argv[],      /* mainの第二引数 */
        int    *optless_num, /* [out] どのオプションにも属さないコンテンツ */
        char ***optless);    /* [out] どのオプションにも属さないコンテンツの数 */

extern opt_group_t* /* groupingOptにより生成されたopt_group_tのポインタ. サイズを超えた場合はNULLが返る. NULLを返す際には動的に確保していたopt_group_tのメモリ領域を全て解放する */
popOptGroup(void); /* groupingOptにより生成されたopt_group_tのポインタを返す関数. 返すopt_group_tのポインタの順番はregOptionPropertyにて登録したpriorityに依存する. */

extern int  /* エラーコードの値. OPTION_SUCCESSが返った時点で以降本関数にて得られるエラーコードは全てOPTION_SUCCESSになる. */
popOptErrcode(void); /* regOptionPropertyにて登録したcontentsCheckerを各オプションに適応して得られたエラーコードを返す関数. 返す順番はregOptionPropertyにて登録したpriorityに依存する. */

extern void
endOptAnalization(void); /* consoleapp/optionにて確保した動的メモリを全て解放する関数 */

#endif
