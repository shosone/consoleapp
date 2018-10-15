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

    if(NULL == (opt_prop_db = (opt_property_db_t *)malloc(sizeof(opt_property_db_t)))){
        return NULL;
    }

    opt_prop_db -> prop_num = prop_num;

    if(NULL == (opt_prop_db->props = (opt_property_t *)calloc(prop_num, sizeof(opt_property_t)))){
        free(opt_prop_db);
        opt_prop_db = NULL;
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
        return OPTION_OPT_NAME_IS_NULL;
    }

    if(content_num_max < content_num_min){
        return OPTION_MIN_BIGGER_THAN_MAX;
    }

	const size_t short_form_len = strlen(short_form) + 1;
    if(NULL == (opt_prop->short_form = (char *)malloc(short_form_len))){
        return OPTION_OUT_OF_MEMORY;
    }
    memcpy(opt_prop->short_form, short_form, short_form_len);

	const size_t long_form_len = strlen(long_form)+ 1;
    if(long_form && NULL == (opt_prop->long_form = (char *)malloc(long_form_len))){
        free(opt_prop->short_form);
        opt_prop->short_form = NULL;
        return OPTION_OUT_OF_MEMORY;
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

static int /* 0:success, 1: out of memory */
decodeOptions(
        opt_property_db_t *db,
        int               org_argc,
        char            **org_argv,
        int              *new_argc_p,
        char           ***new_argv_p)
{
    const int SUCCESS            = 0;
    const int OUT_OF_MEMORY      = 1;

    int ret     = SUCCESS;
    *new_argc_p   = 0;
    (*new_argv_p) = NULL;

    for(int org_argv_i=1; org_argv_i<org_argc; org_argv_i++){
        char *copy_src            = org_argv[org_argv_i];
        char *last_null_ptr       = &copy_src[strlen(copy_src)];
        int   a_part_of_condition = 0;
        char  delim               = '\0';

        for(int db_i = 0; db_i<db->prop_num; db_i++){
            char *memo = strchr(copy_src, '=');
            if(memo){
                if(strcmp(db->props[db_i].long_form, strtok(copy_src, "=")) == 0){
                    a_part_of_condition = 1;
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
			/* const int new_argc = *new_argc_p;
             * if(
             *     (0 < new_argc && (size_t)new_argc < (SIZE_MAX / sizeof(char *)))
             *     || NULL == ((*new_argv_p) = (char **)realloc(*new_argv_p, new_argc * sizeof(char *)))
             * ){  */
            if(!((*new_argv_p) = (char **)reallocarray(*new_argv_p, (*new_argc_p), sizeof(char *)))){
                ret = OUT_OF_MEMORY;
                goto free_and_exit;
            }
			/* char** const new_argv = *new_argv_p;
			 * const size_t copy_src_len = strlen(copy_src);
			 * const size_t copy_src_len_with_null_char = copy_src_len + 1;
             * if(NULL == (new_argv[new_argc -1] = (char *)malloc(copy_src_len_with_null_char))){
             *     ret = OUT_OF_MEMORY;
             *     goto free_and_exit;
             * } */
            if(!((*new_argv_p)[*new_argc_p-1] = (char *)malloc(sizeof(char)*(strlen(copy_src)+1)))){
                ret = OUT_OF_MEMORY;
                goto free_and_exit;
            }
            /* if(delim != '\0'){
			 *     new_argv[new_argc -1][0] = '\n'; // あまり良いやり方ではないかもしれないが、この情報はjudgeDestinationで必要になる
             *     memcpy(&(new_argv[new_argc -1][1]), copy_src, copy_src_len_with_null_char);
             * } */
            if(delim != '\0'){
                (*new_argv_p)[*new_argc_p-1][0] = '\n'; // あまり良いやり方ではないかもしれないが、この情報はjudgeDestinationで必要になる
                strcpy(&((*new_argv_p)[*new_argc_p-1][1]), copy_src);
            }
            /* else{
             *     memcpy(new_argv[new_argc -1], copy_src, copy_src_len_with_null_char);
             * } */
            else{
                strcpy((*new_argv_p)[*new_argc_p-1], copy_src);
            }
            /* if(&copy_src[copy_src_len] != last_null_ptr){
             *     copy_src[copy_src_len] = delim; [> 分割した文字列をもとに戻す <]
             * } */
            if(&copy_src[strlen(copy_src)] != last_null_ptr){
                copy_src[strlen(copy_src)] = delim; /* 分割した文字列をもとに戻す */
            }
        }while(a_part_of_condition && 
                ((NULL != (copy_src = strtok(NULL, ",")) && (delim = ',')) ||
                 ((delim = '\0') && (a_part_of_condition = 0))));
    }

    return SUCCESS;

free_and_exit:
    for(int i=0; i<*new_argc_p; i++){
        free((*new_argv_p)[i]);
        (*new_argv_p)[i] = NULL;
    }
    free(*new_argv_p);
    *new_argv_p = NULL;
    return ret;
}

static int
add2optGrpDB_contents(
        opt_group_db_t *opt_grp_db,
        char *str)
{
    const int SUCCESS       = 0;
    const int OUT_OF_MEMORY = 1;

    opt_grp_db -> optless_num += 1;
    if(
        (0 < opt_grp_db->optless_num && (size_t)opt_grp_db->optless_num < (SIZE_MAX / sizeof(char *)))
        || NULL == (opt_grp_db -> optless = (char **)realloc(opt_grp_db->optless, opt_grp_db->optless_num * sizeof(char *)))){
        return OUT_OF_MEMORY;
    }
    opt_grp_db -> optless[opt_grp_db->optless_num-1] = str;
    return SUCCESS;
}

static int
add2optGrpDB_OptGrps_Option(
        opt_group_db_t *opt_grp_db,
        char       *str)     
{
    const int SUCCESS       = 0;
    const int OUT_OF_MEMORY = 1;

    opt_grp_db -> grp_num += 1;
    /* if(
     *     (0 < opt_grp_db->grp_num && (size_t)opt_grp_db->grp_num < (SIZE_MAX / sizeof(opt_group_t)))
     *     || NULL == (opt_grp_db -> grps = (opt_group_t*)realloc(opt_grp_db->grps, opt_grp_db->grp_num * sizeof(opt_group_t)))){
     *     return OUT_OF_MEMORY;
     * } */
    if(!(opt_grp_db -> grps = (opt_group_t*)reallocarray(opt_grp_db->grps, opt_grp_db->grp_num, sizeof(opt_group_t)))){
        return OUT_OF_MEMORY;
    }
    initOptGroup(&(opt_grp_db -> grps[opt_grp_db->grp_num-1]));
    opt_grp_db -> grps[opt_grp_db->grp_num-1].option = str;
    return SUCCESS;
}

static int
add2optGrpDB_OptGrps_Contents(
        opt_group_db_t *opt_grp_db,
        char       *str)     
{
    const int SUCCESS       = 0;
    const int OUT_OF_MEMORY = 1;

    opt_group_t *current_grp  = &(opt_grp_db -> grps[opt_grp_db->grp_num - 1]);
    current_grp -> content_num++;
    /* if(
     *     (0 < current_grp->content_num && (size_t)current_grp->content_num < (SIZE_MAX / sizeof(char *)))
     *     || NULL == (current_grp->contents = (char **)realloc(current_grp->contents, current_grp->content_num * sizeof(char *)))
     * ){  
     *     return OUT_OF_MEMORY;
     * } */
    if(!(current_grp->contents = (char **)reallocarray(current_grp->contents, current_grp->content_num, sizeof(char *)))){
        return OUT_OF_MEMORY;
    }
    current_grp->contents[current_grp->content_num-1] = str;
    return SUCCESS;
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

static int
checkContentsNum(
        opt_property_db_t *opt_prop_db,
        opt_group_db_t    *opt_grp_db)
{
    /* return values */
    const int SUCCESS = 0;
    const int TOO_MANY_CONTENTS    = 1;
    const int TOO_LITTLE_CONTENTS  = 2;

    for(int i=0; i<opt_grp_db->grp_num; i++){
        char *option = opt_grp_db -> grps[i].option;

        opt_property_t *props = opt_prop_db -> props;
        while(strcmp(props->short_form, option) != 0 && strcmp(props->long_form, option) != 0){
            props++;
        }

        int num = opt_grp_db -> grps[i].content_num;
        int min = props -> content_num_min;
        int max = props -> content_num_max;

        if(num < min){
            return TOO_LITTLE_CONTENTS;
        }

        if(num > max){
            return TOO_MANY_CONTENTS;
        }
    }

    return SUCCESS;
}

typedef enum{
    JD_OPT_GRP_DBs_CONTENTS  =  0,
    JD_OPT_GRPs_CONTENTS     =  1,
    JD_OPT_GRPs_OPTION       =  2,
    JD_DUPLICATE_SAME_OPTION = -1,
    JD_TOO_MANY_CONTENTS     = -2,
    JD_TOO_LITTLE_CONTENTS   = -3,
}judgeDestination_errcode_t;

static int
judgeDestination(
        opt_property_db_t *opt_prop_db,
        char **str)
{
    /* flags */
    static bool opt_grp_dbs_contents_is_empty   = 1;
    static bool lock_opt_grp_dbs_contents       = 0;

    /* memos */
    static int current_options_contents_num     = 0;
    static int current_options_contents_num_max = 0;
    static int current_options_contents_num_min = 0;

    for(int i=0; i<opt_prop_db->prop_num; i++){
        if(strcmp(opt_prop_db->props[i].short_form, *str) == 0 || strcmp(opt_prop_db->props[i].long_form, *str) == 0){
            if(!opt_grp_dbs_contents_is_empty){
                lock_opt_grp_dbs_contents = 1;
            }
            if(opt_prop_db->props[i].appeared_yet){
                return JD_DUPLICATE_SAME_OPTION;
            }
            if(current_options_contents_num < current_options_contents_num_min){
                return OPTION_TOO_LITTLE_CONTENTS;
            }
            opt_prop_db->props[i].appeared_yet = 1;
            current_options_contents_num       = 0;
            current_options_contents_num_max   = opt_prop_db->props[i].content_num_max;
            current_options_contents_num_min   = opt_prop_db->props[i].content_num_min;
            return JD_OPT_GRPs_OPTION;
        }
    }

    /* 文字列の先頭の改行コードはdecodeOptionsにてこの関数のために付属された情報で本来の文字列には先頭の改行コードは存在しない */
    if(*str[0] == '\n'){
        if(current_options_contents_num >= current_options_contents_num_max){
            return OPTION_TOO_MANY_CONTENTS;
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

    if(!lock_opt_grp_dbs_contents){
        opt_grp_dbs_contents_is_empty = 0;
        return JD_OPT_GRP_DBs_CONTENTS;
    }

    return OPTION_TOO_MANY_CONTENTS;
}

int
groupingOpt(
        opt_property_db_t *opt_prop_db,
        int               argc,
        char            **argv,
        opt_group_db_t   **opt_grp_db) 
{
    if(NULL == opt_prop_db){
        return OPTION_OPT_PROP_DB_IS_NULL;
    }

    if(NULL == (*opt_grp_db = (opt_group_db_t *)malloc(sizeof(opt_group_db_t)))){
        return OPTION_OUT_OF_MEMORY;
    }
    initOptGroupDB(*opt_grp_db);

    int    new_argc;
    char **new_argv = NULL;
    int    ret;

    ret = decodeOptions(opt_prop_db, argc, argv, &new_argc, &new_argv);
    if(ret != 0){
        goto free_and_exit;
    }

    for(int i=0; i<new_argc; i++){
        switch(judgeDestination(opt_prop_db, &new_argv[i])){
            case JD_OPT_GRP_DBs_CONTENTS:
                if(add2optGrpDB_contents(*opt_grp_db, new_argv[i]) == 1){
                    ret = OPTION_OUT_OF_MEMORY;
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_CONTENTS:
                if(add2optGrpDB_OptGrps_Contents(*opt_grp_db, new_argv[i]) == 1){
                    ret = OPTION_OUT_OF_MEMORY;
                    goto free_and_exit;
                }
                break;

            case JD_OPT_GRPs_OPTION:
                if(add2optGrpDB_OptGrps_Option(*opt_grp_db, new_argv[i]) == 1){
                    ret = OPTION_OUT_OF_MEMORY;
                    goto free_and_exit;
                }
                break;

            case JD_DUPLICATE_SAME_OPTION: 
                ret = OPTION_DUPLICATE_SAME_OPT;
                goto free_and_exit;

            case JD_TOO_MANY_CONTENTS:
                ret = OPTION_TOO_MANY_CONTENTS;
                goto free_and_exit;

            case JD_TOO_LITTLE_CONTENTS:
                ret = OPTION_TOO_LITTLE_CONTENTS;
                goto free_and_exit;

            default:
                BUG_REPORT();
        }
    }

    ret = checkContentsNum(opt_prop_db, *opt_grp_db);
    switch(ret){
        case 1:
            ret = OPTION_TOO_MANY_CONTENTS;
            goto free_and_exit;

        case 2:
            ret = OPTION_TOO_LITTLE_CONTENTS;
            goto free_and_exit;

        case 0:
        default:
            break;
    }

    adaptContentsChecker(opt_prop_db, *opt_grp_db);
    return OPTION_SUCCESS;

free_and_exit:
    freeOptGroupDB(*opt_grp_db);
    *opt_grp_db = NULL;
    return ret;
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
