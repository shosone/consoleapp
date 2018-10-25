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

#include "option.h"
#include "defines.h"
#include "errmsg.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

/* ==================================== structures =================================== */

/* NOTE: opt_group_t is defined in option.h */

/* プログラムで使用できるオプションの情報を保持する構造体 */
typedef struct _opt_property_t{
    char        *short_form;                                           /* オプションの短縮形式. 例えば"-v" */
    char        *long_form;                                            /* オプションの詳細形式. 例えば"--version" */
    int          (*contentsChecker)(char **contents, int content_num); /* オプションに付属するcontentsの正しさを調べるコールバック関数 */
    unsigned int content_num_min;                                      /* オプションに付属するcontentsの最小数 */
    unsigned int content_num_max;                                      /* オプションに付属するcontentsの最大数 */
    int          priority;                                             /* オプションが複数指定された時にどのオプションを優先的に処理するかを明示するためのフィールド. 0が最も高い. */
    bool         appeared_yet;                                         /* 同じオプションがすでに指定されたかチェックするためのメモとして用いる */
}opt_property_t;

/* =========================== global variables ========================= */

static int              prop_num_g = 0;
static opt_property_t **prop_gp    = NULL;

static int           grp_num_g = 0;
static opt_group_t **grp_gp    = NULL;

static int  errcode_memo_num_g = 0;
static int *errcode_memo_g     = NULL;

/* =========================== static functions ========================= */

/* used for initialized opt_property_t's contentsChecker */
static int 
alwaysReturnTrue(
        char **contents,
        int  content_num)
{
    (void)contents;
    (void)content_num;
    return 0;
}

static void
freeOptGroup(
        opt_group_t *opt_grp)
{
    for(int i=0; i<opt_grp->content_num; i++){
        free(&(opt_grp->contents[i]));
        opt_grp->contents[i] = NULL;
    }
    free(opt_grp->contents);
    opt_grp->contents = NULL;
}

static void
freeOptProp(
        opt_property_t *opt_prop)
{
    free(opt_prop -> short_form);
    opt_prop -> short_form = NULL;
    free(opt_prop -> long_form);
    opt_prop -> long_form = NULL;
}

/* TODO: テスト */
static void
sortErrcodeMemo(void)
{
    int compare(const void *a, const void *b){
        if(*(int *)a > *(int *)b){
            return 1;
        }
        return 0;
    }

    qsort(errcode_memo_g, errcode_memo_num_g, sizeof(int), compare);
}

/* TODO: テスト */
static void
sortOptGroup(void)
{
    int compare(const void *a, const void *b){
        if(((opt_group_t*)a)->priority > ((opt_group_t*)b)->priority){
            return 1;
        }
        return 0;
    }

    qsort(grp_gp, grp_num_g, sizeof(opt_group_t*), compare);
}

/* TODO: テスト */
static void
initOptGroupT(
        opt_group_t *grp)
{
    grp -> priority    = 0;
    grp -> content_num = 0;
    grp -> contents    = NULL;
}

/* TODO: テスト */
static void
initOptPropertyT(
        opt_property_t *prop)
{
    prop -> short_form      = NULL;
    prop -> long_form       = NULL;
    prop -> contentsChecker = alwaysReturnTrue;
    prop -> content_num_min = 0;
    prop -> content_num_max = 0;
    prop -> priority        = 0;
    prop -> appeared_yet    = false;
}

/* TODO: テスト */
static int 
decodeOptions(
        int               org_argc,
        char            **org_argv,
        int              *new_argc_p,
        char           ***new_argv_p)
{
    *new_argc_p   = 0;
    (*new_argv_p) = NULL;

    for(int org_argv_i=1; org_argv_i<org_argc; org_argv_i++){
        char *copy_src             = org_argv[org_argv_i];
        char *last_null_ptr        = &copy_src[strlen(copy_src)];
        bool   a_part_of_condition = false;
        char  delim                = '\0';

        /* このブロック終了後にa_part_of_conditionがtrueになっていたらorg_argv[org_argv_i]は--hoge=geho形式で--hogeがオプションになっている */
        for(int i = 0; i<prop_num_g; i++){
            char *memo = strchr(copy_src, '=');
            if(memo){
                if(strcmp(prop_gp[i]->long_form, strtok(copy_src, "=")) == 0){
                    a_part_of_condition = true;
                    delim = '=';
                    break;
                }
                else{
                    *memo = '='; /* 分割した文字列をもとに戻す */
                }
            }
        }

        do{
            (*new_argc_p)++;
            if(
                    *new_argc_p > SIZE_MAX/sizeof(char*) || /* overflow check */
                    isNull((*new_argv_p) = (char **)realloc(*new_argv_p, sizeof(char *)*(*new_argc_p))))
            {
                printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                goto free_and_exit;
            }
            const size_t copy_src_len_plus1 = strlen(copy_src) + 1;
            if(isNull((*new_argv_p)[*new_argc_p-1] = (char *)malloc(sizeof(char)*(copy_src_len_plus1)))){
                printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                goto free_and_exit;
            }
            if(delim == ','){
                (*new_argv_p)[*new_argc_p-1][0] = '\n'; // あまり良いやり方ではないかもしれないが、この情報はjudgeDestinationで必要になる
                memcpy(&((*new_argv_p)[*new_argc_p-1][1]), copy_src, copy_src_len_plus1);
            }
            else{
                memcpy((*new_argv_p)[*new_argc_p-1], copy_src, copy_src_len_plus1);
            }
            if(&copy_src[strlen(copy_src)] != last_null_ptr){
                copy_src[strlen(copy_src)] = delim; /* 分割した文字列をもとに戻す */
            }
        }while(a_part_of_condition && 
                ((isNotNull(copy_src = strtok(NULL, ",")) && (delim = ',')) ||
                 ((delim = '\0') && (a_part_of_condition = false))));
    }

    return OPTION_SUCCESS;

free_and_exit:
    for(int i=0; i<*new_argc_p; i++){
        free((*new_argv_p)[i]);
        (*new_argv_p)[i] = NULL;
    }
    free(*new_argv_p);
    *new_argv_p = NULL;
    return OPTION_FAILURE;
}

typedef enum{
    JD_OPT_GRP_DBs_OPTLESS,
    JD_OPT_GRPs_CONTENTS,
    JD_OPT_GRPs_OPTION,
}judgeDestination_errcode_t;

static int /* destination */
judgeDestination(
        char  *str,             /* [out] */
        void **assign_value)    /* [out] */
{
    /* flags */
    static bool opt_grp_dbs_flagless_is_empty = true;
    static bool opt_grp_dbs_flagless_locked   = false;

    /* memos */
    static opt_property_t *current_options_property;
    static int   current_options_contents_num     = 0;
    static int   current_options_contents_num_max = 0;
    static int   current_options_contents_num_min = 0;

    /* if str == NULL, this function was called as finalizing.  */
    if(isNull(str)){
        if(current_options_contents_num < current_options_contents_num_min){
            printUsrErrMsg(OPTION_TOO_LITTLE_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
            return OPTION_FAILURE;
        }
        return OPTION_SUCCESS;
    }

    for(int i=0; i<prop_num_g; i++){
        if(strcmp(prop_gp[i]->short_form, str) == 0 || strcmp(prop_gp[i]->long_form, str) == 0){
            current_options_property = prop_gp[i];
            if(opt_grp_dbs_flagless_is_empty == false){
                opt_grp_dbs_flagless_locked = true;
            }
            if(current_options_property -> appeared_yet){
                printUsrErrMsg(OPTION_DUPLICATE_SAME_OPT, current_options_property -> short_form, current_options_property -> long_form);
                return OPTION_FAILURE;
            }
            if(current_options_contents_num < current_options_contents_num_min){
                printUsrErrMsg(OPTION_TOO_LITTLE_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
                return OPTION_FAILURE;
            }
            prop_gp[i]->appeared_yet = 1;
            current_options_contents_num       = 0;
            current_options_contents_num_max   = current_options_property -> content_num_max;
            current_options_contents_num_min   = current_options_property -> content_num_min;
            *assign_value = &(prop_gp[i] -> priority);
            return JD_OPT_GRPs_OPTION;
        }
    }

    /* 文字列の先頭の改行コードはdecodeOptionsにてこの関数のために付属された情報で本来の文字列には先頭の改行コードは存在しない */
    if(str[0] == '\n'){
        if(current_options_contents_num >= current_options_contents_num_max){
            printUsrErrMsg(OPTION_TOO_MANY_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
            return OPTION_FAILURE;
        }
        else{
            /* 先頭の改行コードを削除 */
            str = &str[1];
            current_options_contents_num++;
            *assign_value = str;
            return JD_OPT_GRPs_CONTENTS;
        }
    }

    if(current_options_contents_num < current_options_contents_num_max){
        current_options_contents_num++;
        *assign_value = str;
        return JD_OPT_GRPs_CONTENTS;
    }

    if(opt_grp_dbs_flagless_locked == false){
        opt_grp_dbs_flagless_is_empty = 0;
        *assign_value = str;
        return JD_OPT_GRP_DBs_OPTLESS;
    }

    printUsrErrMsg(OPTION_TOO_MANY_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
    return OPTION_FAILURE;
}

/* TODO: テスト */
static int /* OPTION_SUCCESS or OPTION_FAILURE */
updateOptless(
        int    *optless_num, /* [out] */
        char ***optless,     /* [out] */
        char   *str)
{
    if(
            (*optless_num+1) > SIZE_MAX/sizeof(char*) || /* overflow check */
            isNull(*optless = (char**)realloc(*optless, sizeof(char*)*(*optless_num+1))))
    {
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return OPTION_FAILURE;
    }

    size_t str_len_plus1 = strlen(str) + 1;
    if(isNull((*optless)[*optless_num] = (char*)malloc(str_len_plus1)))
    {
        *optless = realloc(*optless, sizeof(char*)*(*optless_num)); /* reducing */
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return OPTION_FAILURE;
    }
    memcpy((*optless)[*optless_num], str, str_len_plus1);
    (*optless_num)++;

    return OPTION_SUCCESS;
}

/* TODO: テスト */
static int
updateOptGrpGP(
    judgeDestination_errcode_t  direction,
    void                       *assign_value_p)
{
    switch(direction){
        case JD_OPT_GRPs_OPTION:
            if(
                    (grp_num_g+1) > SIZE_MAX/sizeof(opt_group_t) ||  /* overflow check */
                    isNull(grp_gp = (opt_group_t**)realloc(grp_gp, sizeof(opt_group_t)*(grp_num_g+1))))
            {
                printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                return OPTION_FAILURE;
            }
            if(isNull(grp_gp[grp_num_g] = (opt_group_t*)malloc(sizeof(opt_group_t)))){
                printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                grp_gp = (opt_group_t**)realloc(grp_gp, sizeof(opt_group_t*)*grp_num_g); /* reducing */
                return OPTION_FAILURE;
            }
            initOptGroupT(grp_gp[grp_num_g]);
            grp_gp[grp_num_g] -> priority = *(int *)assign_value_p;
            grp_num_g++;
            break;

        case JD_OPT_GRPs_CONTENTS:
            {
                opt_group_t *grp  = grp_gp[grp_num_g - 1];
                if(
                        (grp->content_num+1) > SIZE_MAX/sizeof(char*) || /* overflow check */
                        isNull(grp->contents = (char **)realloc(grp->contents, sizeof(char *)*(grp->content_num+1))))
                {
                    printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                    return OPTION_FAILURE;
                }
                if(isNull(grp->contents[grp->content_num] = (char*)malloc(strlen(assign_value_p)+1))){
                    printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                    grp->contents = (char **)realloc(grp->contents, sizeof(char*)*(grp->content_num)); /* reducing */
                    return OPTION_FAILURE;
                }
                grp->contents[grp->content_num] = assign_value_p;
                grp -> content_num++;
                break;
            }

        default:
            printDeveloperErrMsg(OPTION_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH);
            return OPTION_FAILURE;
    }

    return OPTION_SUCCESS;
}

/* TODO: テスト */
static int /* OPTION_SUCCESS or OPTION_FAILURE */
adaptContentsChecker(void)
{
    for(int i=0; i<grp_num_g; i++){
        opt_group_t *grp = grp_gp[i];
        for(int j=0; j<prop_num_g; j++){
            opt_property_t *prop = prop_gp[j];
            if(prop->priority == grp->priority){
                if(
                        (errcode_memo_num_g+1) > SIZE_MAX/sizeof(int) || /* overflow check */
                        isNull(errcode_memo_g = realloc(errcode_memo_g, sizeof(int)*(errcode_memo_num_g+1))))
                {
                    goto free_and_exit;
                }
                errcode_memo_g[errcode_memo_num_g++] = prop->contentsChecker(grp->contents, grp->content_num);
                break;
            }
        }
    }

    return OPTION_SUCCESS;

free_and_exit:
    for(int i=0; i<errcode_memo_num_g; i++){
        free(errcode_memo_g);
    }
    return OPTION_FAILURE;
}

/* =========================== extern functions ========================= */

/* TODO: テスト */
int /* OPTION_SUCCESS or OPTION_FAILURE */
regOptProperty( /* opt_property_db_tのエントリを追加する関数 */
    unsigned int priority,        /* popOptGroupにて取り出すオプションの順番(優先度) */
    const char *short_form,      /* [in] オプションの短縮形式 */
    const char *long_form,       /* [in] オプションの詳細形式 */
    int   content_num_min, /* オプションに付属するコンテンツの最少数 */
    int   content_num_max, /* オプションに付属するコンテンツの最大数 */
    int (*contentsChecker)(char **contents, int content_num)) /* オプションのコンテンツをチェックするコールバック関数 */
{
    /* [begin] error check */

    if(isNull(short_form)){
        printProgramerErrMsg(OPTION_SHORT_FORM_IS_NULL);
        return OPTION_FAILURE;
    }

    if(content_num_max < content_num_min){
        printProgramerErrMsg(OPTION_MIN_BIGGER_THAN_MAX);
        return OPTION_FAILURE;
    }

    if(priority == OPTION_SUCCESS){
        printProgramerErrMsg(OPTION_PRIORITY_IS_OPTION_SUCCESS);
        return OPTION_FAILURE;
    }

    for(int i=0; i<prop_num_g; i++){
        if(prop_gp[i]->priority == priority){
            printProgramerErrMsg(OPTION_SAME_PRIORITY);
            return OPTION_FAILURE;
        }
        if(strcmp(prop_gp[i]->short_form, short_form) == 0 || 
           (isNull(long_form) || isNull(prop_gp[i]->long_form) || strcmp(prop_gp[i]->long_form, long_form) == 0))
        {
            printProgramerErrMsg(OPTION_SAME_SHORT_LONG_FORMAT);
            return OPTION_FAILURE;
        }
    }

    /* [done] error check */

    if(
            prop_num_g > SIZE_MAX-1 || /* overflow check */
            isNull(prop_gp = realloc(prop_gp, prop_num_g+1)))
    {
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return OPTION_FAILURE;
    }

    if(isNull(prop_gp[prop_num_g] = malloc(sizeof(opt_property_t)))){
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        prop_gp = realloc(prop_gp, prop_num_g); /* reducing */ 
        return OPTION_FAILURE;
    }

    initOptPropertyT(prop_gp[prop_num_g]);

	const size_t short_form_len = strlen(short_form);
    if(isNull(prop_gp[prop_num_g]->short_form = (char *)malloc(short_form_len))){
        goto free_and_exit;
    }
    memcpy(prop_gp[prop_num_g]->short_form, short_form, short_form_len);

	const size_t long_form_len = strlen(long_form);
    if(long_form && isNull(prop_gp[prop_num_g]->long_form = (char *)malloc(long_form_len))){
        goto free_and_exit;
    }
	memcpy(prop_gp[prop_num_g]->long_form, long_form, long_form_len);

    prop_gp[prop_num_g]->content_num_min = content_num_min;
    prop_gp[prop_num_g]->content_num_max = content_num_max;
    prop_gp[prop_num_g]->priority        = priority;
    if(isNotNull(contentsChecker)){
        prop_gp[prop_num_g]->contentsChecker = contentsChecker;
    }

    prop_num_g++;

    return OPTION_SUCCESS;

free_and_exit:
    printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
    free(prop_gp[prop_num_g]->short_form);
    free(prop_gp[prop_num_g]->long_form);
    free(prop_gp[prop_num_g]);
    prop_gp = realloc(prop_gp, prop_num_g); /* reducing */ 
    return OPTION_FAILURE;
}

/* TODO: テスト */
int /* OPTION_SUCCESS or OPTION_FAILURE */
groupingOpt( /* cliより取得したmainの引数であるargc, argvとregOptionPropertyにて登録したオプション情報をもとにオプションをグルーピングする関数 */
    int     argc,        /* mainの第一引数 */
    char   *argv[],      /* mainの第二引数 */
    int    *optless_num, /* [out] どのオプションにも属さないコンテンツ */
    char ***optless)     /* [out] どのオプションにも属さないコンテンツの数 */
{
    /* [begin] error check */

    if(isNull(prop_gp)){
        printProgramerErrMsg(OPTION_PROP_GP_IS_NULL);
        return OPTION_FAILURE;
    }

    /* [done] error check */

    int    new_argc;
    char **new_argv = NULL;

    if(decodeOptions(argc, argv, &new_argc, &new_argv) != OPTION_SUCCESS){
        goto free_and_exit;
    }

    for(int i=0; i<new_argc; i++){
        void *assign_value = NULL;
        switch(judgeDestination(new_argv[i], &assign_value)){
            case JD_OPT_GRP_DBs_OPTLESS:
                if(updateOptless(optless_num, optless, assign_value) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_CONTENTS:
                if(updateOptGrpGP(JD_OPT_GRPs_CONTENTS, assign_value) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_OPTION:
                if(updateOptGrpGP(JD_OPT_GRPs_OPTION, assign_value) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case OPTION_FAILURE: 
                goto free_and_exit;

            default:
                printDeveloperErrMsg(OPTION_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH);
                goto free_and_exit;
        }
    }

    /* TODO: 動くには動くがあんまりいいやり方ではない. 可読性のためにリファクタが必要 */
    if (judgeDestination(NULL, NULL) == OPTION_FAILURE){
        goto free_and_exit;
    }

    adaptContentsChecker();
    return OPTION_SUCCESS;

free_and_exit:
    for(int i=0; i<grp_num_g; i++){
        free(grp_gp[i]);
    }
    return OPTION_FAILURE;
}

/* TODO: テスト */
opt_group_t* /* groupingOptにより生成されたopt_group_tのポインタ. サイズを超えた場合はNULLが返る. NULLを返す際には動的に確保していたopt_group_tのメモリ領域を全て解放する */
popOptGroup(void) /* groupingOptにより生成されたopt_group_tのポインタを返す関数. 返すopt_group_tのポインタの順番はregOptionPropertyにて登録したpriorityに依存する. */
{
    static int cnt = 0;

    if(cnt == 0){
        sortOptGroup();
    }

    opt_group_t *ret = NULL;
    if(cnt < grp_num_g){
        ret = grp_gp[cnt];
        cnt++;
    }

    return ret;
}

/* TODO: テスト */
int  /* エラーコードの値. OPTION_SUCCESSが返った時点で以降本関数にて得られるエラーコードは全てOPTION_SUCCESSになる. */
popOptErrcode(void) /* regOptionPropertyにて登録したcontentsCheckerを各オプションに適応して得られたエラーコードを返す関数. 返す順番はregOptionPropertyにて登録したpriorityに依存する. */
{
    static int  cnt = 0;

    if(cnt == 0){
        sortErrcodeMemo();
    }

    int ret = OPTION_SUCCESS;
    if(cnt < errcode_memo_num_g){
        ret = errcode_memo_g[cnt];
        cnt++;
    }

    return ret;
}

/* TODO: テスト */
void
endOptAnalization(void) /* consoleapp/optionにて確保した動的メモリを全て解放する関数 */
{
    for(int i=0; i<prop_num_g; i++){
        freeOptProp(prop_gp[i]);
        prop_gp[i] = NULL;
    }
    free(prop_gp);
    prop_gp = NULL;

    for(int i=0; i<grp_num_g; i++){
        freeOptGroup(grp_gp[i]);
        grp_gp[i] = NULL;
    }
    free(grp_gp);
    grp_gp = NULL;

    free(errcode_memo_g);
}
