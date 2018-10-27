#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "../src/option.h"

#define ARE_NUMBER_ERR 100

/* priority */
enum {
    /* high */
    HELP = 1,
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
    int    errcode     = OPTION_SUCCESS;

    regOptProperty(HELP,    "-h", "--help",    0, 0,       NULL);
    regOptProperty(VERSION, "-v", "--version", 0, 0,       NULL);
    regOptProperty(PRINT,   "-p", "--print",   1, INT_MAX, NULL);
    regOptProperty(ADD,     "-a", "--add",     2, 2,       areNumber);
    regOptProperty(SUB,     "-s", "--sub",     2, 2,       areNumber);
    regOptProperty(MUL,     "-m", "--mul",     2, 2,       areNumber);
    regOptProperty(DIV,     "-d", "--div",     2, 2,       areNumber);

    ret  = groupingOpt(argc, argv, &optless_num, &optless);
    if(ret != OPTION_SUCCESS){
        fprintf(stderr, "errno: %d, %s\n", option_errno, option_errmsg);
        fprintf(stderr, "sample: an error occurred while parsing options.\n");
        return 0;
    }

    errcode = popOptErrcode();
    while(errcode != OPTION_SUCCESS){
        switch(errcode){
            default:
                break;
        }
        errcode = popOptErrcode();
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

    endOptAnalization();
    return 0;
}

void printUsage(void){
    printf("Usage: sample\n");
    printf("\t-h,--help          print this help\n");
    printf("\t-v,--version       print version\n");
    printf("\t-p <str..>,\n");
    printf("\t--print=<str..>    print <str>\n");
    printf("\n");
}

void printVersion(void){
    printf("version 0.0.0\n");
}

int areNumber(char **contents, int content_num){
    for(int i=0;i<content_num;i++){
        char *p = contents[content_num];
        while(1){
            if(!isdigit(*p)){
                return ARE_NUMBER_ERR;
            }
            if(*p == '\0'){
                break;
            }
        }
    }
    return OPTION_SUCCESS;
}
