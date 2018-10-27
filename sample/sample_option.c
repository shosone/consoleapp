#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "../src/option.h"

#define OPTION_ERR_BASE         50
#define USR_DEFINITION_ERR_BASE 100

enum {
    ARE_NUMBER_ERR = 1, /* because OPTION_SUCCESS is 0 */
};

/* priority */
enum {
    /* high */
    HELP,
    VERSION,
    PRINT,
    ADD,
    SUB,
    MUL,
    DIV,
    /* low */
};

void printUsage(void);
void printVersion(void);
int areNumber(char **contents, int content_num);

int main(int argc, char *argv[]){

    int    optless_num = 0;
    char **optless     = NULL;
    int    ret         = OPTION_SUCCESS;

    regOptProperty(HELP,    "-h", "--help",    0, 0,       NULL);
    regOptProperty(VERSION, "-v", "--version", 0, 0,       NULL);
    regOptProperty(PRINT,   "-p", "--print",   1, INT_MAX, NULL);
    regOptProperty(ADD,     "-a", "--add",     2, 2,       areNumber);
    regOptProperty(SUB,     "-s", "--sub",     2, 2,       areNumber);
    regOptProperty(MUL,     "-m", "--mul",     2, 2,       areNumber);
    regOptProperty(DIV,     "-d", "--div",     2, 2,       areNumber);

    if(groupingOpt(argc, argv, &optless_num, &optless) == OPTION_FAILURE){
        fprintf(stderr, "option error: %s\n", option_errmsg);
        ret = option_errno + OPTION_ERR_BASE;
        goto free_and_exit;
    }

    ret = popOptErrcode();
    switch(ret){
        case ARE_NUMBER_ERR:
            ret += USR_DEFINITION_ERR_BASE;
            fprintf(stderr, "option error: not a number\n");
            goto free_and_exit;

        default:
            break;
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

            case ADD:
                {
                    int a = atoi(opt_grp_p->contents[0]);
                    int b = atoi(opt_grp_p->contents[1]); 
                    printf("%d + %d = %d\n", a, b, a+b);
                }
                break;

            case SUB:
                {
                    int a = atoi(opt_grp_p->contents[0]);
                    int b = atoi(opt_grp_p->contents[1]); 
                    printf("%d - %d = %d\n", a, b, a-b);
                }
                break;

            case MUL:
                {
                    int a = atoi(opt_grp_p->contents[0]);
                    int b = atoi(opt_grp_p->contents[1]); 
                    printf("%d * %d = %d\n", a, b, a*b);
                }
                break;

            case DIV:
                {
                    int a = atoi(opt_grp_p->contents[0]);
                    int b = atoi(opt_grp_p->contents[1]); 
                    printf("%d / %d = %d\n", a, b, a/b);
                }
                break;

            default:
                fprintf(stderr, "error: there is a bug at %d in %s\n", __LINE__, __FILE__);
                break;
        }
    }

    if(optless != NULL){
        printf("these optionless contents are ignored in this sample.\n");
        for(int i=0; i<optless_num; i++){
            printf("%s, ", optless[i]);
        }
        printf("\b\b \n");
    }

free_and_exit:
    endOptAnalization();
    return ret;
}

void printUsage(void){
    printf("Usage: sample\n");
    printf("\t-h,--help          print this help\n");
    printf("\t-v,--version       print version\n");
    printf("\t-p <str..>,\n");
    printf("\t--print=<str..>    print <str>\n");
    printf("\t-a <num> <num>,\n");
    printf("\t--add=<num>,<num>  execute <num> + <num>\n");
    printf("\t-s <num> <num>,\n");
    printf("\t--sub=<num>,<num>  execute <num> - <num>\n");
    printf("\t-m <num> <num>,\n");
    printf("\t--mul=<num>,<num>  execute <num> * <num>\n");
    printf("\t-d <num> <num>,\n");
    printf("\t--div=<num>,<num>  execute <num> / <num>\n");
    printf("\n");
}

void printVersion(void){
    printf("version 0.0.0\n");
}

int areNumber(char **contents, int content_num){
    for(int i=0;i<content_num;i++){
        char *p = contents[i];
        while(1){
            if(*p == '\0'){
                break;
            }
            if(!isdigit(*p)){
                return ARE_NUMBER_ERR;
            }
            p++;
        }
    }
    return OPTION_SUCCESS;
}
