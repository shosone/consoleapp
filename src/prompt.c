#include "prompt.h"

const char DEFAULT_SC_HEAD[]       = {0x01};
const char DEFAULT_SC_TAIL[]       = {0x05};
const char DEFAULT_SC_NEXT_BLOCK[] = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x44};
const char DEFAULT_SC_PREV_BLOCK[] = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x43};
const char DEFAULT_SC_COMPLETION[] = {0x09};
const char DEFAULT_SC_DIVE_HIST[]  = {0x1b, 0x5b, 0x41};
const char DEFAULT_SC_FLOAT_HIST[] = {0x1b, 0x5b, 0x42};
static const char right[]          = {0x1b, 0x5b, 0x43};
static const char left[]           = {0x1b, 0x5b, 0x44};
static const char delete[]         = {0x1b, 0x5b, 0x33, 0x7e, 0x1b};


static char
getch(void)
{
    char   buf         = 0;
    struct termios old = {0};

    fflush(stdout);

    if(tcgetattr(0, &old)<0){
        perror("tcsetattr()");
    }

    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;

    if(tcsetattr(0, TCSANOW, &old)<0){
        perror("tcsetattr ICANON");
    }

    if(read(0,&buf,1)<0){
        perror("read()");
    }
    
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;

    if(tcsetattr(0, TCSADRAIN, &old)<0){
        perror ("tcsetattr ~ICANON");
    }

    return buf;
}

sRwhCtx*
genRwhCtx(int history_size)
{
    sRwhCtx *ctx = NULL;

    if(!(ctx = (sRwhCtx*)malloc(sizeof(sRwhCtx)))){
        return NULL;
    }

    ctx -> history       = NULL;
    ctx -> sc_head       = NULL;
    ctx -> sc_tail       = NULL;
    ctx -> sc_next_block = NULL;
    ctx -> sc_prev_block = NULL;
    ctx -> sc_completion = NULL;
    ctx -> sc_dive_hist  = NULL;
    ctx -> sc_float_hist = NULL;

    if(!(ctx -> history = (sRingBuf *)malloc(sizeof(sRingBuf)))){
        goto free_and_exit;
    }
    ctx -> history -> buf  = NULL;
    ctx -> history -> size = history_size;
    ctx -> history -> head = 0;
    ctx -> history -> tail = 0;

    if(!(ctx -> history -> buf = (char **)calloc(history_size, sizeof(char *)))){
        goto free_and_exit;
    }
    memset(ctx -> history -> buf, 0, history_size);

    if(!(ctx -> sc_head       = malloc(sizeof(char)*strlen(DEFAULT_SC_HEAD)))){
        goto free_and_exit;
    }

    if(!(ctx -> sc_tail       = malloc(sizeof(char)*strlen(DEFAULT_SC_TAIL)))){
        goto free_and_exit;
    }

    if(!(ctx -> sc_next_block = malloc(sizeof(char)*strlen(DEFAULT_SC_NEXT_BLOCK)))){
        goto free_and_exit;
    }

    if(!(ctx -> sc_prev_block = malloc(sizeof(char)*strlen(DEFAULT_SC_PREV_BLOCK)))){
        goto free_and_exit;
    }

    if(!(ctx -> sc_completion = malloc(sizeof(char)*strlen(DEFAULT_SC_COMPLETION)))){
        goto free_and_exit;
    }

    if(!(ctx -> sc_dive_hist  = malloc(sizeof(char)*strlen(DEFAULT_SC_DIVE_HIST)))){
        goto free_and_exit;
    }

    if(!(ctx -> sc_float_hist = malloc(sizeof(char)*strlen(DEFAULT_SC_FLOAT_HIST)))){
        goto free_and_exit;
    }

    strcpy(ctx -> sc_head,       DEFAULT_SC_HEAD);
    strcpy(ctx -> sc_tail,       DEFAULT_SC_TAIL);
    strcpy(ctx -> sc_next_block, DEFAULT_SC_NEXT_BLOCK);
    strcpy(ctx -> sc_prev_block, DEFAULT_SC_PREV_BLOCK);
    strcpy(ctx -> sc_completion, DEFAULT_SC_COMPLETION);
    strcpy(ctx -> sc_dive_hist,  DEFAULT_SC_DIVE_HIST);
    strcpy(ctx -> sc_float_hist, DEFAULT_SC_FLOAT_HIST);
    return ctx;

free_and_exit:
    free(ctx -> sc_float_hist);
    free(ctx -> sc_dive_hist);
    free(ctx -> sc_completion);
    free(ctx -> sc_next_block);
    free(ctx -> sc_prev_block);
    free(ctx -> sc_dive_hist);
    free(ctx -> sc_tail);
    free(ctx -> sc_head);
    if(!(ctx -> history)){
        free(ctx -> history -> buf);
    }
    free(ctx -> history);
    free(ctx);
    return NULL;
}

static void
push2Ringbuf(
        sRingBuf   *rb,
        char *str)
{
    /* buf is empty */
    if(rb->buf[rb->head] == NULL){
        rb->buf[rb->head] = str;
    }
    /* buf is full */
    else if(rb->head == rb->tail+1 || rb->tail == rb->size-1){
        free(rb -> buf[rb -> head]);
        rb -> buf[rb -> head] = str;

        if(rb->head == rb->size-1){
            rb -> head = 0;
            rb -> tail++;
        }
        else if(rb->tail == rb->size-1){
            rb -> head = 1;
            rb -> tail = 0;
        }
        else{
            rb -> head++;
            rb -> tail++;
        }
    }
    /* buf is halfway */
    else{
        rb -> tail++;
        rb -> buf[rb -> tail] = str;
    }
}

static char *
readRingBuf(
        sRingBuf *rb,
        int depth)
{
    int idx = rb -> tail - depth;
    if(idx < 0){
        idx += rb -> size;
    }
    return rb -> buf[idx];
}

static int
judgeShortCut(
        sRwhCtx    *ctx,
        const char *str)
{
    /* return values */
    const int NOT_SHORT_CUT = 0;
    const int UNKNOWN_YET   = 1;
    const int SC_HEAD       = 2;
    const int SC_TAIL       = 3;
    const int SC_NEXT_BLOCK = 4;
    const int SC_PREV_BLOCK = 5;
    const int SC_COMPLETION = 6;
    const int SC_DIVE_HIST  = 7;
    const int SC_FLOAT_HIST = 8;
    const int RIGHT         = 9;  /* ショートカットでは無いがカーソル右が制御信号なので */ 
    const int LEFT          = 10; /* ショートカットでは無いがカーソル左が制御信号なので */ 
    const int DELETE        = 11; /* ショートカットでは無いがカーソル左が制御信号なので */ 

    /* flags */
    int sc_head_possibility       = 1;
    int sc_tail_possibility       = 1;
    int sc_next_block_possibility = 1;
    int sc_prev_block_possibility = 1;
    int sc_completion_possibility = 1;
    int sc_dive_hist_possibility  = 1;
    int sc_float_hist_possibility = 1;
    int right_possibility         = 1;
    int left_possibility          = 1;
    int delete_possibility        = 1;

    int str_len = strlen(str);

    for(int i=0; i<strlen(str); i++){
        int sc_len;
        if(sc_head_possibility){
            sc_len = strlen(ctx->sc_head);
            sc_head_possibility = sc_len >= str_len && ctx->sc_head[i] == str[i];
            if(sc_head_possibility && sc_len == str_len){
                return SC_HEAD;
            }
        }
        if(sc_tail_possibility){
            sc_len = strlen(ctx->sc_tail);
            sc_tail_possibility = sc_len >= str_len && ctx->sc_tail[i] == str[i];
            if(sc_tail_possibility && sc_len == str_len){
                return SC_TAIL;
            }
        }
        if(sc_next_block_possibility){
            sc_len = strlen(ctx->sc_next_block);
            sc_next_block_possibility = sc_len >= str_len && ctx->sc_next_block[i] == str[i];
            if(sc_next_block_possibility && sc_len == str_len){
                return SC_NEXT_BLOCK;
            }
        }
        if(sc_prev_block_possibility){
            sc_len = strlen(ctx->sc_prev_block);
            sc_prev_block_possibility = sc_len >= str_len && ctx->sc_prev_block[i] == str[i];
            if(sc_prev_block_possibility && sc_len == str_len){
                return SC_PREV_BLOCK;
            }
        }
        if(sc_completion_possibility){
            sc_len = strlen(ctx->sc_completion);
            sc_completion_possibility = sc_len >= str_len && ctx->sc_completion[i] == str[i];
            if(sc_completion_possibility && sc_len == str_len){
                return SC_COMPLETION;
            }
        }
        if(sc_dive_hist_possibility){
            sc_len = strlen(ctx->sc_dive_hist);
            sc_dive_hist_possibility = sc_len >= str_len && ctx->sc_dive_hist[i] == str[i];
            if(sc_dive_hist_possibility && sc_len == str_len){
                return SC_DIVE_HIST;
            }
        }
        if(sc_float_hist_possibility){
            sc_len = strlen(ctx->sc_float_hist);
            sc_float_hist_possibility = sc_len >= str_len && ctx->sc_float_hist[i] == str[i];
            if(sc_float_hist_possibility && sc_len == str_len){
                return SC_FLOAT_HIST;
            }
        }
        if(right_possibility){
            sc_len = strlen(right);
            right_possibility = sc_len >= str_len && right[i] == str[i];
            if(right_possibility && sc_len == str_len){
                return RIGHT;
            }
        }
        if(left_possibility){
            sc_len = strlen(left);
            left_possibility = sc_len >= str_len && left[i] == str[i];
            if(left_possibility && sc_len == str_len){
                return LEFT;
            }
        }
        if(delete_possibility){
            sc_len = strlen(delete);
            delete_possibility = sc_len >= str_len && delete[i] == str[i];
            if(delete_possibility && sc_len == str_len){
                return DELETE;
            }
        }
    }

    if(sc_head_possibility || sc_tail_possibility ||
    sc_next_block_possibility || sc_prev_block_possibility ||
    sc_completion_possibility || sc_dive_hist_possibility ||
    sc_float_hist_possibility || right_possibility ||
    left_possibility || delete_possibility)
    {
        return UNKNOWN_YET;
    }

    return NOT_SHORT_CUT;
}

char *
rwh(
        sRwhCtx    *ctx,
        const char *prompt) /* in */
{
    char *line = NULL;
    char *tmp  = NULL;

    printf("%s", prompt);
    fflush(stdout);

    while(1){
        char ch = getch();
        switch(ch){
            case '\n':
                if(!(line = (char *)realloc(line, sizeof(char)*(line == NULL ? 1 : strlen(line)+1)))){
                    return NULL;
                }
                push2Ringbuf(ctx->history, line);
                printf("\n");
                printf("strcmp(\"quit\", line) = %d\n", strcmp(line, "quit")); /* debug */ 
                printf("return \"%s\"\n", line); /* debug */ 
                return line;

            case 0x7f: /* backspace */
                sprintf(line, "%s\b \b", line);
                printf("\b \b");
                break;

            default:
                if(!(tmp = (char *)realloc(tmp, sizeof(char)*(tmp == NULL ? 1 : strlen(tmp)+1)))){
                    return NULL;
                }
                sprintf(tmp, "%s%c", tmp, ch);
                switch(judgeShortCut(ctx, tmp)){
                    case 0: /* NOT_SHORT_CUT  */
                        if(!(line = (char *)realloc(line, sizeof(char)*(line == NULL ? 1 : strlen(line)+1)))){
                            return NULL;
                        }
                        sprintf(line, "%s%c", line, ch);
                        printf("%c", ch);
                        goto free_and_break;

                    case 1: /* UNKNOWN_YET */
                        /* nothing to do */
                        break;

                    case 2: /* SC_HEAD        */
                        sprintf(line, "%s\r", line);
                        printf("\r");
                        goto free_and_break;

                    case 3: /* SC_TAIL        */
                        sprintf(line, "%s%s", line, right);
                        printf("%s", right);
                        goto free_and_break;

                    case 4: /* SC_NEXT_BLOCK  */
                        goto free_and_break;

                    case 5: /* SC_PREV_BLOCK  */
                        goto free_and_break;

                    case 6: /* SC_COMPLETION  */
                        goto free_and_break;

                    case 7: /* SC_DIVE_HIST   */
                        goto free_and_break;

                    case 8: /* SC_FLOAT_HIST  */
                        goto free_and_break;

                    case 9: /* RIGHT  */
                        sprintf(line, "%s%c%c%c", line, right[0], right[1], right[2]);
                        printf("%c%c%c", right[0], right[1], right[2]);
                        goto free_and_break;

                    case 10: /* LEFT  */
                        sprintf(line, "%s%s", line, left);
                        for(int i=0; i<strlen(left); i++){
                            printf("%c", left[i]);
                        }
                        goto free_and_break;

                    case 11: /* DELETE */
                        sprintf(line, "%s%s", line, " \b");
                        printf(" \b");
                        goto free_and_break;

                    default:
                        BUG_REPORT();
                        goto free_and_break;

                    free_and_break:
                        free(tmp);
                        tmp = NULL;
                        break;
                }
                break;
        }
        fflush(stdout);
    }
}

void
freeRwhCtx(sRwhCtx *ctx){
    for(int i=0; i<ctx->history->size; i++){
        free(ctx -> history -> buf[i]);
    }
    free(ctx -> history -> buf);
    free(ctx -> history);
    free(ctx -> sc_head);
    free(ctx -> sc_tail);
    free(ctx -> sc_next_block);
    free(ctx -> sc_prev_block);
    free(ctx -> sc_completion);
    free(ctx -> sc_dive_hist);
    free(ctx -> sc_float_hist);
    free(ctx);
}

