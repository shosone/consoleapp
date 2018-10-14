int chkOptInteractive(char **contents, int dont_care){
    (void)dont_care;
    const int SUCCESS        = 0;
    const int ILLIGAL_NUMBER = 1;

    int num = atoi(contents[0]);
    if(num < 1){
        return ILLIGAL_NUMBER;
    }
    return SUCCESS;
}

#if DEBUG
void debugInfo1(int groupingOpt_ret, opt_group_db_t *opt_grp_db){
    printf("#################### debug info 1 ########################\n");
    printf("groupingOpt returns %d\n", groupingOpt_ret);
    if(opt_grp_db == NULL){
        printf("opt_grp_db is NULL\n");
        printf("#########################################################\n");
        return;
    }
    int    flagless_num = opt_grp_db -> optless_num;
    int    grp_num      = opt_grp_db -> grp_num;
    char **optless      = opt_grp_db -> optless;
    printf("opt_grp_db -> flagless_num = %d\n", flagless_num);
    for(int i=0; i<flagless_num;i++){
        printf("opt_grp_db -> flagless[%d] = %s\n", i, optless[i]);
    }
    printf("opt_grp_db -> opt_grp_num = %d\n", grp_num);
    for(int i=0; i<opt_grp_db->grp_num; i++){
        char  *flag        = opt_grp_db -> grps[i].option;
        int    content_num = opt_grp_db -> grps[i].content_num;
        char **contents    = opt_grp_db -> grps[i].contents;
        int    err_code    = opt_grp_db -> grps[i].err_code;
        printf("opt_grp_db -> opt_grps[%d].err_code = %d\n", i, err_code);
        printf("opt_grp_db -> opt_grps[%d].flag = %s\n", i, flag);
        printf("opt_grp_db -> opt_grps[%d].content_num = %d\n", i, content_num);
        for(int j=0;j<content_num;j++){
            printf("opt_grp_db -> opt_grps[%d].contents[%d] = %s\n", i, j, contents[j]);
        }
    }
    printf("#########################################################\n");
}
#endif
