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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#define LIBCONSOLE_APP_VERSION "0.0"

#ifndef isNull
#define isNull(p)    ((p) == NULL)
#endif

#ifndef isNotNull
#define isNotNull(p) ((p) != NULL)
#endif

/* use printUsrErrMsg(errcode, option_or_content_name) */
typedef enum{
    OPTION_DUPLICATE_SAME_OPT, 
    OPTION_TOO_MANY_CONTENTS, 
    OPTION_TOO_LITTLE_CONTENTS, 
}option_usr_errcode_t;

/* use printProgramerErrMsg(errcode) */
typedef enum{
    OPTION_OPT_NAME_IS_NULL,
    OPTION_MIN_BIGGER_THAN_MAX, 
    OPTION_OPT_PROP_DB_IS_NULL, 
}option_programer_errcode_t;

/* use printDeveloperErrMsg(errcode) */
typedef enum{
    OPTION_OUT_OF_MEMORY, 
    OPTION_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH,
}option_subroutine_errcode_t;

static const char *option_usr_errmsg[] = {
    "%sduplicate same option %s(%s).",
    "%sthe number of contents of option %s(%s) is too many.",
    "%sthe number of contents of option %s(%s) is too little.",
};

static const char *option_programer_errmsg[] = {
    "%sopt_property_t\'s field short_form cannot be NULL. please check 2nd argument of regOptProp().",
    "%sopt_property_t\'s filed content_num_min bigger than content_num_max. please check 4th and 5th argument of regOptProp().",
    "%sopt_property_t pointer is NULL.",
};

static const char *option_developer_errmsg[] = {
    "%sout of memory occurred.",
    "%sunexpected constant value in switch statement.",
};

#define printUsrErrMsg(errcode, short_form, long_form)\
    fprintf(stderr, option_usr_errmsg[errcode], "option error: ", short_form, isNull(long_form) ? " \b\b" : long_form);\
    fprintf(stderr, "\n")

#define printProgramerErrMsg(errcode)\
    fprintf(stderr, option_programer_errmsg[errcode], "usage error (%s@libconsoleapp.a): ", __func__);\
    fprintf(stderr, "\n")

#define printDeveloperErrMsg(errcode)\
    fprintf(stderr, option_developer_errmsg[errcode], "there may be a bug in libconsoleapp.a (;_;): ");\
    fprintf(stderr, " version: %s\n", LIBCONSOLE_APP_VERSION);\
    fprintf(stderr, " file: %s\n", __FILE__);\
    fprintf(stderr, " function: %s\n", __func__);\
    fprintf(stderr, " line no: %d\n", __LINE__);\
    fprintf(stderr, " please give us a bug info. (https://github.com/shosone/consoleapp)")

static int 
alwaysReturnTrue(
        char **contents,
        int  content_num)
{
    (void)contents;
    (void)content_num;
    return 0;
}

opt_property_db_t
*genOptPropDB(
        int prop_num)
{
    opt_property_db_t *opt_prop_db = NULL;

    if(prop_num == 0){
        return NULL;
    }

    if(isNull(opt_prop_db = (opt_property_db_t *)malloc(sizeof(opt_property_db_t)))){
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return NULL;
    }

    opt_prop_db -> prop_num = prop_num;

    if(isNull(opt_prop_db->props = (opt_property_t *)calloc(prop_num, sizeof(opt_property_t)))){
        free(opt_prop_db);
        opt_prop_db = NULL;
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return NULL;
    }

    for(int i=0; i<prop_num; i++){
        opt_prop_db -> props[i].short_form       = NULL;
        opt_prop_db -> props[i].long_form        = NULL;
        opt_prop_db -> props[i].contents_checker = alwaysReturnTrue;
        opt_prop_db -> props[i].appeared_yet     = 0;
    }

    return opt_prop_db;
}

int
regOptProp(
        opt_property_db_t *db,
        char             *short_form,
        char             *long_form,
        int             content_num_min,
        int             content_num_max,
        int             (*contents_checker)(char **contents, int content_num))
{
    static int idx = 0;
    opt_property_t *opt_prop = &(db -> props[idx]);

    if(short_form == NULL){
        printProgramerErrMsg(OPTION_OUT_OF_MEMORY);
        return OPTION_FAILURE;
    }

    if(content_num_max < content_num_min){
        printProgramerErrMsg(OPTION_MIN_BIGGER_THAN_MAX);
        return OPTION_FAILURE;
    }

	const size_t short_form_len = strlen(short_form);
    if(isNull(opt_prop->short_form = (char *)malloc(short_form_len))){
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return OPTION_FAILURE;
    }
    memcpy(opt_prop->short_form, short_form, short_form_len);

	const size_t long_form_len = strlen(long_form);
    if(long_form && isNull(opt_prop->long_form = (char *)malloc(long_form_len))){
        free(opt_prop->short_form);
        opt_prop->short_form = NULL;
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return OPTION_FAILURE;
    }
	memcpy(opt_prop->long_form, long_form, long_form_len);
    
    if(contents_checker){
        opt_prop->contents_checker = contents_checker;
    }

    opt_prop->appeared_yet = 0;
    opt_prop->content_num_min = content_num_min;
    opt_prop->content_num_max = content_num_max;
    idx++;

    return OPTION_SUCCESS;
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

void
freeOptPropDB(
        opt_property_db_t *db)
{
    for(int i=0; i < db->prop_num; i++){
        freeOptProp(&(db->props[i]));
    }
    free(db);
    db = NULL;
}

/* ============================================== */

static void
initOptGroupDB(
        opt_group_db_t *opt_grp_db)
{
    opt_grp_db -> grp_num      = 0;
    opt_grp_db -> grps         = NULL;
    opt_grp_db -> optless_num  = 0;
    opt_grp_db -> optless      = NULL;
}

static void
initOptGroup(
        opt_group_t *grp)
{
    grp -> option      = NULL;
    grp -> content_num = 0;
    grp -> contents    = NULL;
    grp -> err_code    = 0;
}

static int 
decodeOptions(
        opt_property_db_t *db,
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
        for(int db_i = 0; db_i<db->prop_num; db_i++){
            char *memo = strchr(copy_src, '=');
            if(memo){
                if(strcmp(db->props[db_i].long_form, strtok(copy_src, "=")) == 0){
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
            if(isNull((*new_argv_p) = (char **)reallocarray(*new_argv_p, (*new_argc_p), sizeof(char *))))
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

static int
judgeDestination(
        opt_property_db_t *opt_prop_db,
        char             **str)
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
    if(str == NULL){
            if(current_options_contents_num < current_options_contents_num_min){
                printUsrErrMsg(OPTION_TOO_LITTLE_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
                return OPTION_FAILURE;
            }
            return OPTION_SUCCESS;
    }

    for(int i=0; i<opt_prop_db->prop_num; i++){
        if(strcmp(opt_prop_db->props[i].short_form, *str) == 0 || strcmp(opt_prop_db->props[i].long_form, *str) == 0){
            current_options_property = &(opt_prop_db -> props[i]);
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
            opt_prop_db->props[i].appeared_yet = 1;
            current_options_contents_num       = 0;
            current_options_contents_num_max   = current_options_property -> content_num_max;
            current_options_contents_num_min   = current_options_property -> content_num_min;
            return JD_OPT_GRPs_OPTION;
        }
    }

    /* 文字列の先頭の改行コードはdecodeOptionsにてこの関数のために付属された情報で本来の文字列には先頭の改行コードは存在しない */
    if(*str[0] == '\n'){
        if(current_options_contents_num >= current_options_contents_num_max){
            printUsrErrMsg(OPTION_TOO_MANY_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
            return OPTION_FAILURE;
        }
        else{
            /* 先頭の改行コードを削除 */
            *str = &(*str[1]);
            current_options_contents_num++;
            return JD_OPT_GRPs_CONTENTS;
        }
    }

    if(current_options_contents_num < current_options_contents_num_max){
        current_options_contents_num++;
        return JD_OPT_GRPs_CONTENTS;
    }

    if(opt_grp_dbs_flagless_locked == false){
        opt_grp_dbs_flagless_is_empty = 0;
        return JD_OPT_GRP_DBs_OPTLESS;
    }

    printUsrErrMsg(OPTION_TOO_MANY_CONTENTS, current_options_property -> short_form, current_options_property -> long_form);
    return OPTION_TOO_MANY_CONTENTS;
}

static int
updateOptGrpDB(
        judgeDestination_errcode_t direction,
        opt_group_db_t *opt_grp_db,
        char           *str)
{
    switch(direction){
        case JD_OPT_GRP_DBs_OPTLESS:
            opt_grp_db -> optless_num += 1;
            if(isNull(opt_grp_db -> optless = (char **)reallocarray(opt_grp_db->optless, opt_grp_db->optless_num, sizeof(char *)))){
                printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                return OPTION_OUT_OF_MEMORY;
            }
            opt_grp_db -> optless[opt_grp_db->optless_num-1] = str;
            break;

        case JD_OPT_GRPs_OPTION:
            opt_grp_db -> grp_num += 1;
            if(isNull(opt_grp_db -> grps = (opt_group_t*)reallocarray(opt_grp_db->grps, opt_grp_db->grp_num, sizeof(opt_group_t)))){
                printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                return OPTION_OUT_OF_MEMORY;
            }
            initOptGroup(&(opt_grp_db -> grps[opt_grp_db->grp_num-1]));
            opt_grp_db -> grps[opt_grp_db->grp_num-1].option = str;
            break;

        case JD_OPT_GRPs_CONTENTS:
            {
                opt_group_t *current_grp  = &(opt_grp_db -> grps[opt_grp_db->grp_num - 1]);
                current_grp -> content_num++;
                if(isNull(current_grp->contents = (char **)reallocarray(current_grp->contents, current_grp->content_num, sizeof(char *)))){
                    printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
                    return OPTION_OUT_OF_MEMORY;
                }
                current_grp->contents[current_grp->content_num-1] = str;
                break;
            }

        default:
            printDeveloperErrMsg(OPTION_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH);
            return OPTION_UNEXPECTED_CONSTANT_VALUE_IN_SWITCH;
    }

    return OPTION_SUCCESS;
}

static void
adaptContentsChecker(
        opt_property_db_t *opt_prop_db,
        opt_group_db_t    *opt_grp_db)
{
    for(int i=0; i<opt_grp_db->grp_num; i++){
        opt_group_t *grp = &(opt_grp_db -> grps[i]);
        for(int j=0; j<opt_prop_db->prop_num; j++){
            opt_property_t *prop = &(opt_prop_db -> props[j]);
            if(strcmp(prop->short_form, grp->option) == 0 || strcmp(prop->long_form, grp->option) == 0){
                grp->err_code = prop->contents_checker(grp->contents, grp->content_num);
                break;
            }
        }
    }
}

opt_group_db_t*
genOptGrpDB(
        opt_property_db_t *opt_prop_db,
        int               argc,
        char            **argv)
{
    opt_group_db_t *grp_db_p = NULL;

    if(isNull(opt_prop_db)){
        printProgramerErrMsg(OPTION_OPT_PROP_DB_IS_NULL);
        return NULL;
    }

    if(isNull(grp_db_p = (opt_group_db_t *)malloc(sizeof(opt_group_db_t)))){
        printDeveloperErrMsg(OPTION_OUT_OF_MEMORY);
        return NULL;
    }
    initOptGroupDB(grp_db_p);

    int    new_argc;
    char **new_argv = NULL;

    if(decodeOptions(opt_prop_db, argc, argv, &new_argc, &new_argv) != OPTION_SUCCESS){
        goto free_and_exit;
    }

    for(int i=0; i<new_argc; i++){
        switch(judgeDestination(opt_prop_db, &new_argv[i])){
            case JD_OPT_GRP_DBs_OPTLESS:
                if(updateOptGrpDB(JD_OPT_GRP_DBs_OPTLESS, grp_db_p, new_argv[i]) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_CONTENTS:
                if(updateOptGrpDB(JD_OPT_GRPs_CONTENTS, grp_db_p, new_argv[i]) == OPTION_FAILURE){
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_OPTION:
                if(updateOptGrpDB(JD_OPT_GRPs_OPTION, grp_db_p, new_argv[i]) == OPTION_FAILURE){
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

    adaptContentsChecker(opt_prop_db, grp_db_p);
    return grp_db_p;

free_and_exit:
    freeOptGroupDB(grp_db_p);
    return NULL;
}

static void
freeOptGroup(
        opt_group_t *opt_grp)
{
    free(opt_grp->option);
    opt_grp->option = NULL;
    for(int i=0; i<opt_grp->content_num; i++){
        free(opt_grp->contents[i]);
        opt_grp->contents[i] = NULL;
    }
    free(opt_grp->contents);
    opt_grp->contents = NULL;
}

void
freeOptGroupDB(
        opt_group_db_t *opt_grp_db)
{
    for(int i=0; i<opt_grp_db->optless_num; i++){
        free(opt_grp_db -> optless[i]);
        opt_grp_db -> optless[i] = NULL;
    }
    free(opt_grp_db -> optless);
    opt_grp_db -> optless = NULL;
    for(int i=0; i<opt_grp_db->grp_num; i++){
        freeOptGroup(&(opt_grp_db->grps[i]));
    }
    free(opt_grp_db -> grps);
    opt_grp_db -> grps = NULL;
    free(opt_grp_db);
    opt_grp_db = NULL;
}
