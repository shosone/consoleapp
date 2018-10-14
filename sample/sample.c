#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/consoleapp.h"

#define DEBUG 1
#include "for_option.c"
#ifndef CONSOLEAPP_DISABLE_PROMPT
#include "for_prompt.c"
#endif

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
            for(int j=0; j<content_num; j++){
                printf("%s\n", contents[j]);
            }
        }
#ifndef CONSOLEAPP_DISABLE_PROMPT
		else if(strcmp(flag, "-i") == 0 || strcmp(flag, "--interactive") == 0){
            interactive(atoi(contents[0]));
        }
#endif
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

#ifndef CONSOLEAPP_DISABLE_PROMPT
void interactive(int hist_entory_size){
    char     *line;
    int       mode = 1;

    const char *commands1[] = {
        "help",
        "quit",
        "ctx",
        "modctx",
        "!echo",
        "!ls",
        "!pwd",
        "!date",
        "!ls -a",
        "!ls -l",
        "!ls -lt",
    };

    const char *commands2[] = {
        "help",
        "ctx",
        "head",
        "tail",
        "next block",
        "prev block",
        "completion",
        "dive hist",
        "float hist",
        "done",
    };

    /* here!! */
    rwhctx_t *ctx1 = genRwhCtx("sample$ "       , hist_entory_size, commands1, sizeof(commands1)/sizeof(char *));
    rwhctx_t *ctx2 = genRwhCtx("modctx@sample$ ", hist_entory_size, commands2, sizeof(commands2)/sizeof(char *));

    printf("input \"help\" to display help\n");

    while(1){
        switch(mode){
            case 1:
                line = rwh(ctx1);

                if(strcmp(line, "help") == 0){
                    interactiveHelp1();
                }
                else if(strcmp(line, "ctx") == 0){
                    interactivePrintCtx(ctx1);
                }
                else if(line[0] == '!'){
                    system(&line[1]);
                }
                else if(strcmp(line, "modctx") == 0){
                    mode = 2;
                }
                else if(strcmp(line, "quit") == 0){
                    goto free_and_exit;
                }
                else{
                    printf("err\n");
                }
                break;

            case 2:
                line = rwh(ctx2);

                if(strcmp(line, "help") == 0){
                    interactiveHelp2();
                }
                else if(strcmp(line, "ctx") == 0){
                    interactivePrintCtx(ctx2);
                }
                else if(strcmp(line, "head") == 0){
                    printf("input new key bind\n");
                    char *kb = readline("head << ");
                    ctx2 -> sc_head = kb;
                }
                else if(strcmp(line, "tail") == 0){
                    printf("input new key bind\n");
                    char *kb = readline("tail << ");
                    ctx2 -> sc_tail = kb;
                }
                else if(strcmp(line, "next block") == 0){
                    printf("input new key bind\n");
                    char *kb = readline("next block << ");
                    ctx2 -> sc_next_block = kb;
                }
                else if(strcmp(line, "prev block") == 0){
                    printf("input new key bind\n");
                    char *kb = readline("prev block << ");
                    ctx2 -> sc_prev_block = kb;
                }
                else if(strcmp(line, "completion") == 0){
                    printf("input new key bind\n");
                    char *kb = readline("completion << ");
                    ctx2 -> sc_completion = kb;
                }
                else if(strcmp(line, "dive hist") == 0){
                    printf("input new key bind\n");
                    char *kb = readline("dive hist << ");
                    ctx2 -> sc_dive_hist = kb;
                }
                else if(strcmp(line, "float hist") == 0){
                    printf("input new key bind\n");
                    char *kb = readline("float hist << ");
                    ctx2 -> sc_float_hist = kb;
                }
                else if(strcmp(line, "done") == 0){
                    mode = 1;
                }
                else{
                    printf("err\n");
                }
                break;
        }
    }

free_and_exit:
    freeRwhCtx(ctx1);
    freeRwhCtx(ctx2);
}
#endif
