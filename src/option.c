#include "option.h"

static int 
alwaysReturnTrue(
        char **contents,
        int  content_num)
{
    return 0;
}

sFLAG_PROPERY_DB
*genFlagPropDB(
        int prop_num)
{
    sFLAG_PROPERY_DB *flag_prop_db;

    if(prop_num == 0){
        return NULL;
    }

    if(!(flag_prop_db = (sFLAG_PROPERY_DB *)malloc(sizeof(sFLAG_PROPERY_DB)))){
        return NULL;
    }

    flag_prop_db -> prop_num = prop_num;

    if(!(flag_prop_db->props = (sFLAG_PROPERY *)calloc(prop_num, sizeof(sFLAG_PROPERY)))){
        free(flag_prop_db);
        return NULL;
    }

    for(int i=0; i<prop_num; i++){
        flag_prop_db -> props[i].short_form       = NULL;
        flag_prop_db -> props[i].long_form        = NULL;
        flag_prop_db -> props[i].contents_checker = alwaysReturnTrue;
        flag_prop_db -> props[i].appeared_yet     = 0;
    }

    return flag_prop_db;
}

int
addFlagProp2DB(
        sFLAG_PROPERY_DB *db,
        char             *short_form,
        char             *long_form,
        int             content_num_min,
        int             content_num_max,
        int             (*contents_checker)(char **contents, int content_num))
{
    /* return values */
    const int SUCCESS             = 0;
    const int FLAG_NAME_IS_NULL   = 1;
    const int OUT_OF_MEMORY       = 2;
    const int MIN_BIGGER_THAN_MAX = 3;

    static int idx = 0;
    sFLAG_PROPERY *flag_prop = &(db -> props[idx]);

    if(short_form == NULL){
        return FLAG_NAME_IS_NULL;
    }

    if(content_num_max < content_num_min){
        return MIN_BIGGER_THAN_MAX;
    }

    if(!(flag_prop->short_form = (char *)malloc(strlen(short_form)))){
        return OUT_OF_MEMORY;
    }
    strcpy(flag_prop->short_form, short_form);

    if(long_form && !(flag_prop->long_form = (char *)malloc(sizeof(char)*strlen(long_form)))){
        free(flag_prop->short_form);
        return OUT_OF_MEMORY;
    }
    strcpy(flag_prop->long_form, long_form);
    
    if(contents_checker){
        flag_prop->contents_checker = contents_checker;
    }

    flag_prop->appeared_yet = 0;
    flag_prop->content_num_min = content_num_min;
    flag_prop->content_num_max = content_num_max;
    idx++;

    return SUCCESS;
}

static void
freeFlagProp(
        sFLAG_PROPERY *flag_prop)
{
    free(flag_prop -> short_form);
    free(flag_prop -> long_form);
}

void
freeFlagPropDB(
        sFLAG_PROPERY_DB *db)
{
    for(int i=0; i < db->prop_num; i++){
        freeFlagProp(&(db->props[i]));
    }
    free(db);
}

/* ============================================== */

static void
initOptGroupDB(
        sOPT_GROUP_DB *opt_grp_db)
{
    opt_grp_db -> grp_num      = 0;
    opt_grp_db -> grps         = NULL;
    opt_grp_db -> flagless_num = 0;
    opt_grp_db -> flagless     = NULL;
}

static void
initOptGroup(
        sOPT_GROUP *grp)
{
    grp -> flag        = NULL;
    grp -> content_num = 0;
    grp -> contents    = NULL;
    grp -> err_code    = 0;
}

static int /* 0:success, 1: out of memory */
decodeOptions(
        sFLAG_PROPERY_DB *db,
        int               org_argc,
        char            **org_argv,
        int              *new_argc,
        char           ***new_argv)
{
    const int SUCCESS            = 0;
    const int OUT_OF_MEMORY      = 1;

    int ret     = SUCCESS;
    *new_argc   = 0;
    (*new_argv) = NULL;

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
            (*new_argc)++;
            if(!((*new_argv) = (char **)reallocarray((*new_argv), (*new_argc), sizeof(char *)))){
                ret = OUT_OF_MEMORY;
                goto free_and_exit;
            }
            if(!((*new_argv)[*new_argc-1] = (char *)malloc(sizeof(char)*(strlen(copy_src)+1)))){
                ret = OUT_OF_MEMORY;
                goto free_and_exit;
            }
            if(delim != '\0'){
                (*new_argv)[*new_argc-1][0] = '\n'; // あまり良いやり方ではないかもしれないが、この情報はjudgeDestinationで必要になる
                strcpy(&((*new_argv)[*new_argc-1][1]), copy_src);
            }
            else{
                strcpy((*new_argv)[*new_argc-1], copy_src);
            }
            if(&copy_src[strlen(copy_src)] != last_null_ptr){
                copy_src[strlen(copy_src)] = delim; /* 分割した文字列をもとに戻す */
            }
        }while(a_part_of_condition && 
                (((copy_src = strtok(NULL, ",")) && (delim = ',')) ||
                 ((delim = '\0') && (a_part_of_condition = 0))));
    }

    return SUCCESS;

free_and_exit:
    for(int i=0; i<*new_argc; i++){
        free((*new_argv)[i]);
    }
    free(*new_argv);
    return ret;
}

static int
add2optGrpDB_contents(
        sOPT_GROUP_DB *opt_grp_db,
        char *str)
{
    const int SUCCESS       = 0;
    const int OUT_OF_MEMORY = 1;

    opt_grp_db -> flagless_num += 1;
    if(!(opt_grp_db -> flagless = (char **)reallocarray(opt_grp_db->flagless, opt_grp_db->flagless_num, sizeof(char *)))){
        return OUT_OF_MEMORY;
    }
    opt_grp_db -> flagless[opt_grp_db->flagless_num-1] = str;
    return SUCCESS;
}

static int
add2optGrpDB_OptGrps_Flag(
        sOPT_GROUP_DB *opt_grp_db,
        char       *str)     
{
    const int SUCCESS       = 0;
    const int OUT_OF_MEMORY = 1;

    opt_grp_db -> grp_num += 1;
    if(!(opt_grp_db -> grps = (sOPT_GROUP*)reallocarray(opt_grp_db->grps, opt_grp_db->grp_num, sizeof(sOPT_GROUP)))){
        return OUT_OF_MEMORY;
    }
    initOptGroup(&(opt_grp_db -> grps[opt_grp_db->grp_num-1]));
    opt_grp_db -> grps[opt_grp_db->grp_num-1].flag = str;
    return SUCCESS;
}

static int
add2optGrpDB_OptGrps_Contents(
        sOPT_GROUP_DB *opt_grp_db,
        char       *str)     
{
    const int SUCCESS       = 0;
    const int OUT_OF_MEMORY = 1;

    sOPT_GROUP *current_grp  = &(opt_grp_db -> grps[opt_grp_db->grp_num - 1]);
    current_grp -> content_num++;
    if(!(current_grp->contents = (char **)reallocarray(current_grp->contents, current_grp->content_num, sizeof(char *)))){
        return OUT_OF_MEMORY;
    }
    current_grp->contents[current_grp->content_num-1] = str;
    return SUCCESS;
}

static void
adaptContentsChecker(
        sFLAG_PROPERY_DB *flag_prop_db,
        sOPT_GROUP_DB    *opt_grp_db)
{
    for(int i=0; i<opt_grp_db->grp_num; i++){
        sOPT_GROUP *grp = &(opt_grp_db -> grps[i]);
        for(int j=0; j<flag_prop_db->prop_num; j++){
            sFLAG_PROPERY *prop = &(flag_prop_db -> props[j]);
            if(strcmp(prop->short_form, grp->flag) == 0 || strcmp(prop->long_form, grp->flag) == 0){
                grp->err_code = prop->contents_checker(grp->contents, grp->content_num);
                break;
            }
        }
    }
}

static int
checkContentsNum(
        sFLAG_PROPERY_DB *flag_prop_db,
        sOPT_GROUP_DB    *opt_grp_db)
{
    /* return values */
    const int SUCCESS = 0;
    const int TOO_MANY_CONTENTS    = 1;
    const int TOO_LITTLE_CONTENTS  = 2;

    for(int i=0; i<opt_grp_db->grp_num; i++){
        char *flag = opt_grp_db -> grps[i].flag;

        sFLAG_PROPERY *props = flag_prop_db -> props;
        while(strcmp(props->short_form, flag) != 0 && strcmp(props->long_form, flag) != 0){
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

static int
judgeDestination(
        sFLAG_PROPERY_DB *flag_prop_db,
        char **str)
{
    /* return values */
    const int OPT_GRP_DBs_CONTENTS =  0;
    const int OPT_GRPs_CONTENTS    =  1;
    const int OPT_GRPs_FLAG        =  2;
    const int DUPLICATE_SAME_FLAG  = -1;
    const int TOO_MANY_CONTENTS    = -2;
    const int TOO_LITTLE_CONTENTS  = -3;

    /* flags */
    static int opt_grp_dbs_contents_is_empty   = 1;
    static int lock_opt_grp_dbs_contents       = 0;

    /* memos */
    static int current_flags_contents_num     = 0;
    static int current_flags_contents_num_max = 0;
    static int current_flags_contents_num_min = 0;

    for(int i=0; i<flag_prop_db->prop_num; i++){
        if(strcmp(flag_prop_db->props[i].short_form, *str) == 0 || strcmp(flag_prop_db->props[i].long_form, *str) == 0){
            if(!opt_grp_dbs_contents_is_empty){
                lock_opt_grp_dbs_contents = 1;
            }
            if(flag_prop_db->props[i].appeared_yet){
                return DUPLICATE_SAME_FLAG;
            }
            if(current_flags_contents_num < current_flags_contents_num_min){
                return TOO_LITTLE_CONTENTS;
            }
            flag_prop_db->props[i].appeared_yet = 1;
            current_flags_contents_num          = 0;
            current_flags_contents_num_max      = flag_prop_db->props[i].content_num_max;
            current_flags_contents_num_min      = flag_prop_db->props[i].content_num_min;
            return OPT_GRPs_FLAG;
        }
    }

    /* 文字列の先頭の改行コードはdecodeOptionsにてこの関数のために付属された情報で本来の文字列には先頭の改行コードは存在しない */
    if(*str[0] == '\n'){
        if(current_flags_contents_num >= current_flags_contents_num_max){
            return TOO_MANY_CONTENTS;
        }
        else{
            /* 先頭の改行コードを削除 */
            *str = &(*str[1]);
            current_flags_contents_num++;
            return OPT_GRPs_CONTENTS;
        }
    }

    if(current_flags_contents_num < current_flags_contents_num_max){
        current_flags_contents_num++;
        return OPT_GRPs_CONTENTS;
    }

    if(!lock_opt_grp_dbs_contents){
        opt_grp_dbs_contents_is_empty = 0;
        return OPT_GRP_DBs_CONTENTS;
    }

    return TOO_MANY_CONTENTS;
}

int
groupingOpt(
        sFLAG_PROPERY_DB *flag_prop_db,
        int               argc,
        char            **argv,
        sOPT_GROUP_DB   **opt_grp_db) 
{
    /* return values */
    const int SUCCESS              = 0;
    const int OUT_OF_MEMORY        = 1;
    const int FLAG_PROP_DB_IS_NULL = 2;
    const int DUPLICATE_SAME_FLAG  = 3;
    const int TOO_MANY_CONTENTS    = 4;
    const int TOO_LITTLE_CONTENTS  = 5;

    if(!flag_prop_db){
        return FLAG_PROP_DB_IS_NULL;
    }

    if(!(*opt_grp_db = (sOPT_GROUP_DB *)malloc(sizeof(sOPT_GROUP_DB)))){
        return OUT_OF_MEMORY;
    }
    initOptGroupDB(*opt_grp_db);

    int    new_argc;
    char **new_argv;
    int    ret;

    ret = decodeOptions(flag_prop_db, argc, argv, &new_argc, &new_argv);
    if(ret != 0){
        goto free_and_exit;
    }

    for(int i=0; i<new_argc; i++){
        switch(judgeDestination(flag_prop_db, &new_argv[i])){
            case 0: /* OPT_GRP_DBs_CONTENTS */
                if(add2optGrpDB_contents(*opt_grp_db, new_argv[i]) == 1){
                    ret = OUT_OF_MEMORY;
                    goto free_and_exit;
                }
                break;

            case 1: /* OPT_GRPs_CONTENTS */
                if(add2optGrpDB_OptGrps_Contents(*opt_grp_db, new_argv[i]) == 1){
                    ret = OUT_OF_MEMORY;
                    goto free_and_exit;
                }
                break;

            case 2: /* OPT_GRPs_FLAG */
                if(add2optGrpDB_OptGrps_Flag(*opt_grp_db, new_argv[i]) == 1){
                    ret = OUT_OF_MEMORY;
                    goto free_and_exit;
                }
                break;

            case -1: 
                ret = DUPLICATE_SAME_FLAG;
                goto free_and_exit;

            case -2:
                ret = TOO_MANY_CONTENTS;
                goto free_and_exit;

            case -3:
                ret = TOO_LITTLE_CONTENTS;
                goto free_and_exit;

            default:
                BUG_REPORT();
        }
    }

    ret = checkContentsNum(flag_prop_db, *opt_grp_db);
    switch(ret){
        case 1:
            ret = TOO_MANY_CONTENTS;
            goto free_and_exit;

        case 2:
            ret = TOO_LITTLE_CONTENTS;
            goto free_and_exit;

        default:
            break;
    }

    adaptContentsChecker(flag_prop_db, *opt_grp_db);
    return SUCCESS;

free_and_exit:
    freeOptGroupDB(*opt_grp_db);
    *opt_grp_db = NULL;
    return ret;
}

static void
freeOptGroup(
        sOPT_GROUP *opt_grp)
{
    free(opt_grp->flag);
    for(int i=0; i<opt_grp->content_num; i++){
        free(opt_grp->contents[i]);
    }
    free(opt_grp->contents);
    free(opt_grp);
}

void
freeOptGroupDB(
        sOPT_GROUP_DB *opt_grp_db)
{
    for(int i=0; i<opt_grp_db->flagless_num; i++){
        free(opt_grp_db -> flagless[i]);
    }
    free(opt_grp_db->flagless);
    for(int i=0; i<opt_grp_db->grp_num; i++){
        freeOptGroup(&(opt_grp_db->grps[i]));
    }
    free(opt_grp_db->grps);
    free(opt_grp_db);
}
