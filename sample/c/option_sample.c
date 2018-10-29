#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "../../src/option.h"

enum {
    ARE_NUMBER_ERR = 500, 
};

/* priority */
enum {
    /* high */
    VERSION,
    HELP,
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
        fprintf(stderr, "option error: %s\n", optionErrno2msg(option_errno));
        ret = option_errno;
        goto free_and_exit;
    }

    switch(popOptErrcode()){
        case ARE_NUMBER_ERR:
            fprintf(stderr, "option error: not a number\n");
            ret = ARE_NUMBER_ERR;
            goto free_and_exit;

        default:
            break;
    }

    for(opt_group_t *grp_p = popOptGroup(); grp_p != NULL; grp_p = popOptGroup()){
        switch(grp_p -> priority){
            case HELP:
                printUsage();
                break;

            case VERSION:
                printVersion();
                break;

            case PRINT:
                for(int i=0; i<grp_p->content_num; i++){
                    printf("%s\n", grp_p->contents[i]);
                }
                break;

            case ADD:
                {
                    int a = atoi(grp_p->contents[0]);
                    int b = atoi(grp_p->contents[1]); 
                    printf("%d + %d = %d\n", a, b, a+b);
                }
                break;

            case SUB:
                {
                    int a = atoi(grp_p->contents[0]);
                    int b = atoi(grp_p->contents[1]); 
                    printf("%d - %d = %d\n", a, b, a-b);
                }
                break;

            case MUL:
                {
                    int a = atoi(grp_p->contents[0]);
                    int b = atoi(grp_p->contents[1]); 
                    printf("%d * %d = %d\n", a, b, a*b);
                }
                break;

            case DIV:
                {
                    int a = atoi(grp_p->contents[0]);
                    int b = atoi(grp_p->contents[1]); 
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
    printf("Usage: sample_option\n");
    printf("[short format option]\n");
    printf("\t-h              print this help\n");
    printf("\t-v              print version of liboption.a\n");
    printf("\t-p <str..>      print specified strs\n");
    printf("\t-a <num> <num>  add two specified numbers\n");
    printf("\t-s <num> <num>  sub two specified numbers\n");
    printf("\t-m <num> <num>  mul two specified numbers\n");
    printf("\t-d <num> <num>  div two specified numbers\n");
    printf("\n");
    printf("[long format option]\n");
    printf("these option\'s cantens can be specified \"option=contents,contents,..\" or \"option contents contents ..\"\n");
    printf("\t--print         same as -p\n");
    printf("\t--add           same as -a\n");
    printf("\t--sub           same as -s\n");
    printf("\t--mul           same as -m\n");
    printf("\t--div           same as -d\n");
    printf("\n");
}

void printVersion(void){
    printf("liboption.a v%s\n", CONSOLEAPP_OPTION_VERSION);
    printf("Copyright (c) This software is provided under the MIT license.\n");
    printf("See \"https://opensource.org/licenses/MIT\" for details.\n");
    printf("\n");
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
