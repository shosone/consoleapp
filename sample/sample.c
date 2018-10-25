#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <readline/readline.h>
#include "../src/option.h"
#include "../src/prompt.h"

#define DEBUG 1

/* priority */
enum {
    /* high */
    HELP = 1,
    VERSION,
    PRINT,
    INTERACTIVE,
    /* low */
};

/* error code */
enum {
    SAMPLE_SUCCESS = 0,
    INTERACTIVE_ILLIGAL_NUMBER,
};

void printUsage(void);
void printVersion(void);
void interactive(int hist_entory_size);

int chkOptInteractive(char **contents, int dont_care){
    (void)dont_care;

    int num = atoi(contents[0]);
    if(num < 1){
        return INTERACTIVE_ILLIGAL_NUMBER;
    }
    return SAMPLE_SUCCESS;
}

int main(int argc, char *argv[]){

    regOptProperty(HELP,        "-h", "--help",        0, 0,       NULL);
    regOptProperty(VERSION,     "-v", "--version",     0, 0,       NULL);
    regOptProperty(PRINT,       "-p", "--print",       1, INT_MAX, NULL);
    regOptProperty(INTERACTIVE, "-i", "--interactive", 1, 1,       chkOptInteractive);

    int    optless_num = 0;
    char **optless     = NULL;
    if(groupingOpt(argc, argv, &optless_num, &optless) == OPTION_FAILURE){
        fprintf(stderr, "sample: an error occurred while parsing options.\n");
        return 0;
    }

    int errcode = OPTION_SUCCESS;
    while((errcode = popOptErrcode()) != OPTION_SUCCESS){
        switch(errcode){
            case INTERACTIVE_ILLIGAL_NUMBER:
                fprintf(stderr, "error: the history size specified with the option -i(--interactive) is an invalid value\n");
                exit(1);
        }
    }

    opt_group_t *opt_grp_p = NULL;
    while((opt_grp_p = popOptGroup()) != NULL){
        switch(opt_grp_p -> priority){
            case HELP:
                printUsage();
                break;

            case VERSION:
                printVersion();
                break;

            case PRINT:
                for(int i=0; i<opt_grp_p->content_num; i++){
                    printf("%s\n", opt_grp_p->contents[i]);
                }
                break;

            case INTERACTIVE:
                interactive(atoi(opt_grp_p->contents[0]));
                break;

            default:
                fprintf(stderr, "error: there is a bug at %d in %s\n", __LINE__, __FILE__);
                break;
        }
    }

    if(optless != NULL){
        printf("these optionless contents are ignored in this sample.\n");
        for(int i=0; i<optless_num; i++){
            printf("%s\n", optless[i]);
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

/* for consoleapp/prompt ============================================ */

#ifndef CONSOLEAPP_DISABLE_PROMPT
void interactiveHelp1(void){
    printf("+------------------------------------------------------------------------+\n");
    printf("|help:           print this help                                         |\n");
    printf("|quit:           quit interactive mode                                   |\n");
    printf("|ctx:            print context info                                      |\n");
    printf("|modctx:         modify shortcut settings                                |\n");
    printf("|![some string]: execute \"[some string]\" as a shell command.             |\n");
    printf("|                                                                        |\n");
    printf("|NOTE: these key bind is able to change by modifying rwh_ctx_t\'s fields. |\n");
    printf("|short cuts:                                                             |\n");
    printf("|    Ctl-a:  jump to head                                                |\n");
    printf("|    Ctl-e:  jump to tail                                                |\n");
    printf("|    Ctl-→ : jump to next separation                                     |\n");
    printf("|    Ctl-← : jump to previous separation                                 |\n");
    printf("|history operation:                                                      |\n");
    printf("|    ↑ : go to the past                                                  |\n");
    printf("|    ↓ : go to the future                                                |\n");
    printf("|    tab: completion                                                     |\n");
    printf("+------------------------------------------------------------------------+\n");
}

void interactiveHelp2(void){
    printf("+------------------------------------------+\n");
    printf("|help:        print this help              |\n");
    printf("|ctx:         print context info           |\n");
    printf("|done:        finish modctx mode           |\n");
    printf("|head:        change sc_head key bind      |\n");
    printf("|tail:        change sc_tail key bind      |\n");
    printf("|next block:  change sc_next_block key bind|\n");
    printf("|prev block:  change sc_prev_block key bind|\n");
    printf("|completion:  change sc_completion key bind|\n");
    printf("|dive hist:   change dive hist key bind    |\n");
    printf("|float hist:  change float hist key bind   |\n");
    printf("+------------------------------------------+\n");
}

void interactivePrintCtx(rwhctx_t *ctx){
    printf("history: \n");
    printf(" ↑ old\n");
    for(int i=0; i<ctx->history->entory_num; i++){
        printf("    %s\n", ctx->history->buf[i]);
    }
    printf(" ↓ new\n");
    printf("candidate: \n");
    for(int i=0; i<ctx->candidate->entory_num; i++){
        printf("    %s\n", ctx->candidate->entories[i]);
    }
    printf("sc_head: %s\n", ctx->sc_head);
    printf("sc_tail: %s\n", ctx->sc_tail);
    printf("sc_next_block: %s\n", ctx->sc_next_block);
    printf("sc_prev_block: %s\n", ctx->sc_prev_block);
    printf("sc_completion: %s\n", ctx->sc_completion);
    printf("sc_dive_hist: %s\n", ctx->sc_dive_hist);
    printf("sc_float_hist: %s\n", ctx->sc_float_hist);
}

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
