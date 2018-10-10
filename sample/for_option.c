#include <stdlib.h>
#include "../src/option.h"

int chkOptInteractive(char **contents, int dont_care){
    const int SUCCESS        = 0;
    const int ILLIGAL_NUMBER = 1;

    int num = atoi(contents[0]);
    if(num < 1){
        return ILLIGAL_NUMBER;
    }
    return SUCCESS;
}

void regFlagProp(sFLAG_PROPERY_DB *flag_prop_db){
    addFlagProp2DB(flag_prop_db, "-h", "--help",        0,       0, NULL);
    addFlagProp2DB(flag_prop_db, "-v", "--version",     0,       0, NULL);
    addFlagProp2DB(flag_prop_db, "-p", "--print",       1, INT_MAX, NULL);
    addFlagProp2DB(flag_prop_db, "-i", "--interactive", 1,       1, chkOptInteractive);
}

#if DEBUG
void debugInfo1(int groupingOpt_ret, sOPT_GROUP_DB *opt_grp_db){
    printf("#################### debug info 1 ########################\n");
    printf("groupingOpt returns %d\n", groupingOpt_ret);
    if(opt_grp_db == NULL){
        printf("opt_grp_db is NULL\n");
        printf("#########################################################\n");
        return;
    }
    int    flagless_num = opt_grp_db -> flagless_num;
    int    grp_num      = opt_grp_db -> grp_num;
    char **flagless     = opt_grp_db -> flagless;
    printf("opt_grp_db -> flagless_num = %d\n", flagless_num);
    for(int i=0; i<flagless_num;i++){
        printf("opt_grp_db -> flagless[%d] = %s\n", i, flagless[i]);
    }
    printf("opt_grp_db -> opt_grp_num = %d\n", grp_num);
    for(int i=0; i<opt_grp_db->grp_num; i++){
        char  *flag        = opt_grp_db -> grps[i].flag;
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
