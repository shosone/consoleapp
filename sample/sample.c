#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/option.h"
#include "../src/prompt.h"

#define DEBUG 1
#include "for_option.c"

void printUsage(void);
void printVersion(void);
void interactive(int hist_entory_size);

int main(int argc, char *argv[]){

    opt_property_db_t *opt_prop_db = genOptPropDB(4);
    opt_group_db_t    *opt_grp_db   = NULL;
    int                ret;

    regOptProp(opt_prop_db, "-h", "--help",        0,       0, NULL);
    regOptProp(opt_prop_db, "-v", "--version",     0,       0, NULL);
    regOptProp(opt_prop_db, "-p", "--print",       1, INT_MAX, NULL);
    regOptProp(opt_prop_db, "-i", "--interactive", 1,       1, chkOptInteractive);

    ret = groupingOpt(opt_prop_db, argc, argv, &opt_grp_db);

#if DEBUG
    debugInfo1(ret, opt_grp_db);
#endif

    switch(ret){
        case OPTION_SUCCESS: /* success */
            break;

        case OPTION_OPT_NAME_IS_NULL:
            fprintf(stderr, "error: out of memory\n");
            exit(1);

        case OPTION_OUT_OF_MEMORY:
            fprintf(stderr, "error: flag_prop_db is null\n");
            exit(1);

        case OPTION_DUPLICATE_SAME_OPT:
            fprintf(stderr, "error: duplicate same flag\n");
            exit(1);

        case OPTION_TOO_MANY_CONTENTS:
            fprintf(stderr, "error: too many contents\n");
            exit(1);

        case OPTION_TOO_LITTLE_CONTENTS:
            fprintf(stderr, "error: too little contents\n");
            exit(1);

        default:
            fprintf(stderr, "there is a bug! (line: %d)\n", __LINE__);
            exit(100);
    }

    for(int i=0; i<opt_grp_db->grp_num; i++){
        switch(opt_grp_db->grps[i].err_code){
            case 0: /* success */
                break;

            case 1: 
                fprintf(stderr, "error: the history size \"%s\" specified with the option \"%s\" is an invalid value\n", opt_grp_db->grps[i].contents[0], opt_grp_db->grps[i].option);
                exit(2);
                break;

            default:
                fprintf(stderr, "error: there is a bug! (line: %d)\n", __LINE__);
                exit(100);
        }
    }

    for(int i=0;i<opt_grp_db->grp_num;i++){
        char *flag       = opt_grp_db -> grps[i].option;
        char **contents  = opt_grp_db -> grps[i].contents;
        int  content_num = opt_grp_db -> grps[i].content_num;

        if(strcmp(flag, "-h") == 0 || strcmp(flag, "--help") == 0){
            printUsage();
        }
        else if(strcmp(flag, "-v") == 0 || strcmp(flag, "--version") == 0){
            printVersion();
        }
        else if(strcmp(flag, "-p") == 0 || strcmp(flag, "--print") == 0){
            for(int i=0; i<content_num; i++){
                printf("%s\n", contents[i]);
            }
        }
        else if(strcmp(flag, "-i") == 0 || strcmp(flag, "--interactive") == 0){
            interactive(atoi(contents[0]));
        }
    }

    return 0;
}

void printUsage(void){
    printf("Usage: sample\n");
    printf("\t-h,--help                    print this help\n");
    printf("\t-v,--version                 print version\n");
    printf("\t-p <str..>,\n");
    printf("\t--print=<str..>              print <str>\n");
    printf("\t-i <history_size>,\n");
    printf("\t--interactive=<history_size> start as interactive mode\n");
    printf("\n");
}

void printVersion(void){
    printf("version 0.0.0\n");
}

void print(int str_num, char **strs){
    for(int i=0; i<str_num; i++){
        printf("%s\n", strs[i]);
    }
    printf("\n");
}

void interactive(int hist_entory_size){
    rwhctx_t *ctx = genRwhCtx(30);
    char *line;

    printf("input \"help\" to display help\n");

    while(1){
        line = rwh(ctx, ">> ");

        if(strcmp(line, "help") == 0){
            printf("+----------------------------------------------------------------------+\n");
            printf("|help:           print this help                                       |\n");
            printf("|quit:           quit interactive mode                                 |\n");
            printf("|![some string]: execute \"[some string]\" as a shell command.           |\n");
            printf("|                                                                      |\n");
            printf("|NOTE: these key bind is able to change by modifying sRwhCtx\'s fields. |\n");
            printf("|short cuts:                                                           |\n");
            printf("|    Ctl-a:  jump to head                                              |\n");
            printf("|    Ctl-e:  jump to tail                                              |\n");
            printf("|    Ctl-→ : jump to next separation                                   |\n");
            printf("|    Ctl-← : jump to previous separation                               |\n");
            printf("|history operation:                                                    |\n");
            printf("|    ↑ : go to the past                                                |\n");
            printf("|    ↓ : go to the future                                              |\n");
            printf("|    tab: completion                                                   |\n");
            printf("+----------------------------------------------------------------------+\n");
        }
        else if(strcmp(line, "quit") == 0){
            exit(1);
        }
        else if(line[0] == '!'){
            system(&line[1]);
        }
    }

    freeRwhCtx(ctx);
}

