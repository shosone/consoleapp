#include <stdio.h>
#include "../src/prompt.h"

void debugInfo2(sRwhCtx *ctx){
    char **history = ctx -> history;
    printf("#################### debug info #########################\n");
    for(int i=0; history[i]!=NULL && i<ctx->entry_max; i++){
        printf("history[i]: %s\n", history[i]);
    }
    printf("#########################################################\n");
}
