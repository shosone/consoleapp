#ifndef PROMPT_H
#define PROMPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#ifndef BUG_REPORT
#include <stdio.h>
#define BUG_REPORT() (fprintf(stderr, "error: there is a bug! (%s, %s, %d)", __FILE__, __FUNCTION__, __LINE__))
#endif

extern const char DEFAULT_SC_HEAD[];
extern const char DEFAULT_SC_TAIL[];
extern const char DEFAULT_SC_NEXT_BLOCK[];
extern const char DEFAULT_SC_PREV_BLOCK[];
extern const char DEFAULT_SC_COMPLETION[];
extern const char DEFAULT_SC_DIVE_HIST[];
extern const char DEFAULT_SC_FLOAT_HIST[]; 

typedef struct _sRwhCtx{
    int  entry_max;
    char **history;
    char *sc_head;
    char *sc_tail;
    char *sc_next_block;
    char *sc_prev_block;
    char *sc_completion;
    char *sc_dive_hist;
    char *sc_float_hist;
}sRwhCtx;

extern sRwhCtx*
genRwhCtx(int history_size);

extern void
freeRwhCtx(sRwhCtx *ctx);

extern char *
rwh( /* acquire the line entered in the console and keep history by libreadline.a */
        sRwhCtx    *ctx,
        const char *prompt);  /* [in]  string to be output to the console when urging the usr input */

#endif 
