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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "./option.h"

/* ================================ imports from common.c ============================ */

extern void
_consoleappBugReport(
    consoleapp_bugno_t errno);

/* ================================ imports from option_errmsg.c ===================== */

extern void
_printAPIusageErrMsg(
        option_logic_errno_t errno,
        const char*            func_name);

extern void
_makeEndUsrErrMsg(
        option_runtime_errno_t errno,
        char *short_form,
        char *long_form);

/* =========================== global variables ========================= */

static int               _prop_num_g = 0;
static _opt_property_t **_prop_gp    = NULL;

static int            _grp_num_g = 0;
static opt_group_t **_grp_gp     = NULL;

static int  _errcode_memo_num_g = 0;
static int *_errcode_memo_gp    = NULL;

/* =========================== static functions ========================= */

/* used for initialized opt_property_t's contentsChecker */
static int 
_alwaysReturnTrue(
        char **contents,
        int  content_num)
{
    (void)contents;
    (void)content_num;
    return 0;
}

static void
_freeOptGroup(
        opt_group_t *opt_grp)
{
    for(int i=0; i<opt_grp->content_num; i++){
        free(opt_grp->contents[i]);
        opt_grp->contents[i] = NULL;
    }
    free(opt_grp->contents);
    opt_grp->contents = NULL;
}

static void
_freeOptProp(
        _opt_property_t *opt_prop)
{
    free(opt_prop -> short_form);
    opt_prop -> short_form = NULL;
    free(opt_prop -> long_form);
    opt_prop -> long_form = NULL;
}

static int _sortErrcodeMemoCompare(const void *a, const void *b){
    if((*(int *)a) < (*(int *)b)){
        return 1;
    }
    return 0;
}

static void
_sortErrcodeMemo(void)
{
    qsort(_errcode_memo_gp, _errcode_memo_num_g, sizeof(int), _sortErrcodeMemoCompare);
}

static int _sortOptGroupCompare(const void *a, const void *b){
    if((*(opt_group_t**)a)->priority > (*(opt_group_t**)b)->priority){
        return 1;
    }
    return 0;
}
static void
_sortOptGroup(void)
{
    qsort(_grp_gp, _grp_num_g, sizeof(opt_group_t*), _sortOptGroupCompare);
}

static void
_initOptGroupT(
        opt_group_t *grp)
{
    grp -> priority    = 0;
    grp -> content_num = 0;
    grp -> contents    = NULL;
}

static void
_initOptPropertyT(
        _opt_property_t *prop)
{
    prop -> short_form      = NULL;
    prop -> long_form       = NULL;
    prop -> contentsChecker = _alwaysReturnTrue;
    prop -> content_num_min = 0;
    prop -> content_num_max = 0;
    prop -> priority        = 0;
    prop -> appeared_yet    = false;
}

static int 
_decodeOptions(
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
        for(int i = 0; i<_prop_num_g; i++){
            char *memo = strchr(copy_src, '=');
            if(memo){
                if(strcmp(_prop_gp[i]->long_form, strtok(copy_src, "=")) == 0){
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
                    isOverflow4Realloc(*new_argc_p, char*) ||
                    ((*new_argv_p) = (char **)realloc(*new_argv_p, sizeof(char *)*(*new_argc_p))) == NULL)
            {
                /* out of memory should be handled in errno, not in this library */
                goto free_and_exit;
            }
            const size_t copy_src_len_plus1 = strlen(copy_src) + 1;
            if(((*new_argv_p)[*new_argc_p-1] = (char *)malloc(sizeof(char)*(copy_src_len_plus1))) == NULL){
                /* out of memory should be handled in errno, not in this library */
                goto free_and_exit;
            }
            memcpy((*new_argv_p)[*new_argc_p-1], copy_src, copy_src_len_plus1);
            if(delim == ','){
                (*new_argv_p)[*new_argc_p-1][copy_src_len_plus1-1] = '\n'; // あまり良いやり方ではないかもしれないが、この情報はjudgeDestinationで必要になる
            }
            if(&copy_src[strlen(copy_src)] != last_null_ptr){
                copy_src[strlen(copy_src)] = delim; /* 分割した文字列をもとに戻す */
            }
        }while(a_part_of_condition && 
                (((copy_src = strtok(NULL, ",")) != NULL && (delim = ',')) ||
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
_judgeDestination(
        char  *str,             /* [out] */
        void **assign_value)    /* [out] */
{
    /* flags */
    static bool opt_grp_dbs_flagless_is_empty = true;
    static bool opt_grp_dbs_flagless_locked   = false;

    /* memos */
    static _opt_property_t *current_options_property;
    static int   current_options_contents_num     = 0;
    static int   current_options_contents_num_max = 0;
    static int   current_options_contents_num_min = 0;

    /* if str == NULL, this function was called as finalizing.  */
    if(str == NULL){
        if(current_options_contents_num < current_options_contents_num_min){
            _makeEndUsrErrMsg(OPTION_TOO_LITTLE_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
            return OPTION_FAILURE;
        }
        return OPTION_SUCCESS;
    }

    for(int i=0; i<_prop_num_g; i++){
        if(strcmp(_prop_gp[i]->short_form, str) == 0 || strcmp(_prop_gp[i]->long_form, str) == 0){
            current_options_property = _prop_gp[i];
            if(opt_grp_dbs_flagless_is_empty == false){
                opt_grp_dbs_flagless_locked = true;
            }
            if(current_options_property -> appeared_yet){
                _makeEndUsrErrMsg(OPTION_DUPLICATE_SAME_OPT, current_options_property -> short_form, current_options_property -> long_form);
                return OPTION_FAILURE;
            }
            if(current_options_contents_num < current_options_contents_num_min){
                _makeEndUsrErrMsg(OPTION_TOO_LITTLE_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
                return OPTION_FAILURE;
            }
            _prop_gp[i]->appeared_yet = 1;
            current_options_contents_num       = 0;
            current_options_contents_num_max   = current_options_property -> content_num_max;
            current_options_contents_num_min   = current_options_property -> content_num_min;
            *assign_value = &(_prop_gp[i] -> priority);
            return JD_OPT_GRPs_OPTION;
        }
    }

    /* 文字列の先頭の改行コードはdecodeOptionsにてこの関数のために付属された情報で本来の文字列には先頭の改行コードは存在しない */
    const size_t len_minus1 = strlen(str) - 1;
    if(str[len_minus1] == '\n'){
        if(current_options_contents_num >= current_options_contents_num_max){
            _makeEndUsrErrMsg(OPTION_TOO_MANY_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
            return OPTION_FAILURE;
        }
        else{
            /* 終端の改行コードを削除 */
            str[len_minus1] = '\0';
            str = realloc(str, sizeof(char)*(len_minus1+1));
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

    _makeEndUsrErrMsg(OPTION_TOO_MANY_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
    return OPTION_FAILURE;
}

static int /* OPTION_SUCCESS or OPTION_FAILURE */
_updateOptless(
        int    *optless_num, /* [out] */
        char ***optless,     /* [out] */
        char   *str)
{
    if(
            isOverflow4Realloc(*optless_num+1, char*) ||
            (*optless = (char**)realloc(*optless, sizeof(char*)*(*optless_num+1))) == NULL)
    {
        /* out of memory should be handled in errno, not in this library */
        return OPTION_FAILURE;
    }

    size_t str_len_plus1 = strlen(str) + 1;
    if(((*optless)[*optless_num] = (char*)malloc(str_len_plus1)) == NULL)
    {
        *optless = realloc(*optless, sizeof(char*)*(*optless_num)); /* reducing */
        /* out of memory should be handled in errno, not in this library */
        return OPTION_FAILURE;
    }
    memcpy((*optless)[*optless_num], str, str_len_plus1);
    (*optless_num)++;

    return OPTION_SUCCESS;
}

static int
_updateOptGrpGP(
    judgeDestination_errcode_t  direction,
    void                       *assign_value_p)
{
    switch(direction){
        case JD_OPT_GRPs_OPTION:
            if(
                    isOverflow4Realloc(_grp_num_g+1, opt_group_t) ||
                    (_grp_gp = (opt_group_t**)realloc(_grp_gp, sizeof(opt_group_t)*(_grp_num_g+1))) == NULL)
            {
                /* out of memory should be handled in errno, not in this library */
                return OPTION_FAILURE;
            }
            if((_grp_gp[_grp_num_g] = (opt_group_t*)malloc(sizeof(opt_group_t))) == NULL){
                /* out of memory should be handled in errno, not in this library */
                _grp_gp = (opt_group_t**)realloc(_grp_gp, sizeof(opt_group_t*)*_grp_num_g); /* reducing */
                return OPTION_FAILURE;
            }
            _initOptGroupT(_grp_gp[_grp_num_g]);
            _grp_gp[_grp_num_g] -> priority = *(int *)assign_value_p;
            _grp_num_g++;
            break;

        case JD_OPT_GRPs_CONTENTS:
            {
                opt_group_t *grp  = _grp_gp[_grp_num_g - 1];
                if(
                        isOverflow4Realloc(grp->content_num+1, char*) ||
                        (grp->contents = (char **)realloc(grp->contents, sizeof(char *)*(grp->content_num+1))) == NULL)
                {
                /* out of memory should be handled in errno, not in this library */
                    return OPTION_FAILURE;
                }
                if((grp->contents[grp->content_num] = (char*)malloc(strlen(assign_value_p)+1)) == NULL){
                    /* out of memory should be handled in errno, not in this library */
                    grp->contents = (char **)realloc(grp->contents, sizeof(char*)*(grp->content_num)); /* reducing */
                    return OPTION_FAILURE;
                }
                grp->contents[grp->content_num] = assign_value_p;
                grp -> content_num++;
                break;
            }

        default:
            _consoleappBugReport(__CONSOLEAPP_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH);
            return OPTION_FAILURE;
    }

    return OPTION_SUCCESS;
}

static int /* OPTION_SUCCESS or OPTION_FAILURE */
_adaptContentsChecker(void)
{
    for(int i=0; i<_grp_num_g; i++){
        opt_group_t *grp = _grp_gp[i];
        for(int j=0; j<_prop_num_g; j++){
            _opt_property_t *prop = _prop_gp[j];
            if(prop->priority == grp->priority){
                if(
                        isOverflow4Realloc(_errcode_memo_num_g+1, int) ||
                        (_errcode_memo_gp = realloc(_errcode_memo_gp, sizeof(int)*(_errcode_memo_num_g+1))) == NULL)
                {
                    goto free_and_exit;
                }
                _errcode_memo_gp[_errcode_memo_num_g++] = prop->contentsChecker(grp->contents, grp->content_num);
                break;
            }
        }
    }

    return OPTION_SUCCESS;

free_and_exit:
    for(int i=0; i<_errcode_memo_num_g; i++){
        free(_errcode_memo_gp);
    }
    _errcode_memo_gp = 0;
    return OPTION_FAILURE;
}

/* =========================== extern functions ========================= */

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

    if(short_form == NULL){
        _printAPIusageErrMsg(OPTION_SHORT_FORM_IS_NULL, __func__);
        return OPTION_FAILURE;
    }

    if(content_num_max < content_num_min){
        _printAPIusageErrMsg(OPTION_MIN_BIGGER_THAN_MAX, __func__);
        return OPTION_FAILURE;
    }

    for(int i=0; i<_prop_num_g; i++){
        if(_prop_gp[i]->priority == priority){
            _printAPIusageErrMsg(OPTION_SAME_PRIORITY, __func__);
            return OPTION_FAILURE;
        }
        if(strcmp(_prop_gp[i]->short_form, short_form) == 0 || 
           (long_form == NULL || _prop_gp[i]->long_form == NULL || strcmp(_prop_gp[i]->long_form, long_form) == 0))
        {
            _printAPIusageErrMsg(OPTION_SAME_SHORT_LONG_FORMAT, __func__);
            return OPTION_FAILURE;
        }
    }

    /* [done] error check */

    if(
            isOverflow4Realloc(_prop_num_g+1, _opt_property_t*) ||
            (_prop_gp = realloc(_prop_gp, sizeof(_opt_property_t*)*(_prop_num_g+1))) == NULL)
    {
        /* out of memory should be handled in errno, not in this library */
        return OPTION_FAILURE;
    }

    if((_prop_gp[_prop_num_g] = malloc(sizeof(_opt_property_t))) == NULL){
        /* out of memory should be handled in errno, not in this library */
        _prop_gp = realloc(_prop_gp, sizeof(_opt_property_t*)*_prop_num_g); /* reducing */ 
        return OPTION_FAILURE;
    }

    _initOptPropertyT(_prop_gp[_prop_num_g]);

	const size_t short_form_len = strlen(short_form);
    if((_prop_gp[_prop_num_g]->short_form = (char *)malloc(short_form_len)) == NULL){
        goto free_and_exit;
    }
    memcpy(_prop_gp[_prop_num_g]->short_form, short_form, short_form_len);

	const size_t long_form_len = strlen(long_form);
    if(long_form && (_prop_gp[_prop_num_g]->long_form = (char *)malloc(long_form_len)) == NULL){
        goto free_and_exit;
    }
	memcpy(_prop_gp[_prop_num_g]->long_form, long_form, long_form_len);

    _prop_gp[_prop_num_g]->content_num_min = content_num_min;
    _prop_gp[_prop_num_g]->content_num_max = content_num_max;
    _prop_gp[_prop_num_g]->priority        = priority;
    if(contentsChecker != NULL){
        _prop_gp[_prop_num_g]->contentsChecker = contentsChecker;
    }

    _prop_num_g++;

    return OPTION_SUCCESS;

free_and_exit:
    /* out of memory should be handled in errno, not in this library */
    free(_prop_gp[_prop_num_g]->short_form);
    free(_prop_gp[_prop_num_g]->long_form);
    free(_prop_gp[_prop_num_g]);
    _prop_gp = realloc(_prop_gp, sizeof(_opt_property_t)*_prop_num_g); /* reducing */ 
    return OPTION_FAILURE;
}

int /* OPTION_SUCCESS or OPTION_FAILURE */
groupingOpt( /* cliより取得したmainの引数であるargc, argvとregOptionPropertyにて登録したオプション情報をもとにオプションをグルーピングする関数 */
    int     argc,        /* mainの第一引数 */
    char   *argv[],      /* mainの第二引数 */
    int    *optless_num, /* [out] どのオプションにも属さないコンテンツ */
    char ***optless)     /* [out] どのオプションにも属さないコンテンツの数 */
{
    /* [begin] error check */

    if(_prop_gp == NULL){
        _printAPIusageErrMsg(OPTION_PROP_GP_IS_NULL, __func__);
        return OPTION_FAILURE;
    }

    /* [done] error check */

    int    new_argc;
    char **new_argv = NULL;

    if(_decodeOptions(argc, argv, &new_argc, &new_argv) != OPTION_SUCCESS){
        goto free_and_exit;
    }

    for(int i=0; i<new_argc; i++){
        void *assign_value = NULL;
        switch(_judgeDestination(new_argv[i], &assign_value)){
            case JD_OPT_GRP_DBs_OPTLESS:
                if(_updateOptless(optless_num, optless, assign_value) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_CONTENTS:
                if(_updateOptGrpGP(JD_OPT_GRPs_CONTENTS, assign_value) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_OPTION:
                if(_updateOptGrpGP(JD_OPT_GRPs_OPTION, assign_value) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case OPTION_FAILURE: 
                goto free_and_exit;

            default:
                _consoleappBugReport(__CONSOLEAPP_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH);
                goto free_and_exit;
        }
    }

    /* TODO: 動くには動くがあんまりいいやり方ではない. 可読性のためにリファクタが必要 */
    if (_judgeDestination(NULL, NULL) == OPTION_FAILURE){
        goto free_and_exit;
    }

    _adaptContentsChecker();
    return OPTION_SUCCESS;

free_and_exit:
    for(int i=0; i<_grp_num_g; i++){
        free(_grp_gp[i]);
    }
    _grp_num_g = 0;
    return OPTION_FAILURE;
}

opt_group_t* /* groupingOptにより生成されたopt_group_tのポインタ. サイズを超えた場合はNULLが返る. NULLを返す際には動的に確保していたopt_group_tのメモリ領域を全て解放する */
popOptGroup(void) /* groupingOptにより生成されたopt_group_tのポインタを返す関数. 返すopt_group_tのポインタの順番はregOptionPropertyにて登録したpriorityに依存する. */
{
    static int cnt = 0;

    if(cnt == 0){
        _sortOptGroup();
    }

    opt_group_t *ret = NULL;
    if(cnt < _grp_num_g){
        ret = _grp_gp[cnt];
        cnt++;
    }

    return ret;
}

int  /* エラーコードの値. OPTION_SUCCESSが返った時点で以降本関数にて得られるエラーコードは全てOPTION_SUCCESSになる. */
popOptErrcode(void) /* regOptionPropertyにて登録したcontentsCheckerを各オプションに適応して得られたエラーコードを返す関数. 返す順番はregOptionPropertyにて登録したpriorityに依存する. */
{
    static int  cnt = 0;

    if(cnt == 0){
        _sortErrcodeMemo();
    }

    int ret = OPTION_SUCCESS;
    if(cnt < _errcode_memo_num_g){
        ret = _errcode_memo_gp[cnt];
        cnt++;
    }

    return ret;
}

void
endOptAnalization(void) /* consoleapp/optionにて確保した動的メモリを全て解放する関数 */
{
    for(int i=0; i<_prop_num_g; i++){
        _freeOptProp(_prop_gp[i]);
        _prop_gp[i] = NULL;
    }
    free(_prop_gp);
    _prop_gp = NULL;

    for(int i=0; i<_grp_num_g; i++){
        _freeOptGroup(_grp_gp[i]);
        _grp_gp[i] = NULL;
    }
    free(_grp_gp);
    _grp_gp = NULL;

    free(_errcode_memo_gp);
    _prop_num_g = 0;
    _grp_num_g  = 0;
    _errcode_memo_num_g = 0;
}
