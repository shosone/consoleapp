#include <stdio.h>
#include <string.h>
#include "../../src/prompt.h"

#define COMMANDS_SIZE 4
#define HISTORY_SIZE  10

int main(void){

    rwhctx_t *ctx  = NULL;
    char     *line = NULL;

    const char *commands[COMMANDS_SIZE] = {
        "help",
        "version",
        "ctx",
        "quit",
    };

    ctx = genRwhCtx("sample_prompt$", HISTORY_SIZE, commands, COMMANDS_SIZE);

    while(1){
        line = rwh(ctx);
        if(strcmp(line, "help") == 0){
            printf("help:    print this help\n");
            printf("version: print version of libprompt.a\n");
            printf("ctx:     print \"rwhctx_t *ctx\" info\n");
            printf("quit:    quit\n");
        }
        else if(strcmp(line, "version") == 0){
            printf("version: %s\n", CONSOLEAPP_PROMPT_VERSION);
        }
        else if(strcmp(line, "ctx") == 0){
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
        else if(strcmp(line, "quit") == 0){
            break;
        }
        else{
            fprintf(stderr, "error\n");
        }
    }

    freeRwhCtx(ctx);
    return 0;
}
