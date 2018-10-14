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
