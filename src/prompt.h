/* MIT License
 * 
 * Copyright (c) 2018 Sho Sone
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef PROMPT_H
#define PROMPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>

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

/* structure for ring buffer. this is used for rwh_ctx_t's member. there is no need for user to know. */
typedef struct _ringbuf_t{
    char **buf;         /* buffer for entories */
    int    size;        /* max size of buffer */
    int    head;        /* buffer index at an oldest entory */
    int    tail;        /* buffer index at an newest entory */
    int    entory_num;  /* number of entories */
}ringbuf_t;

/* structure for holding candidates at completion. */
typedef struct _completion_t{
    char** entories;   /* entories are sorted in ascending order */
    int    entory_num; /* number of entories */
}completion_t;

/* structure for preserve context for rwh(). */
typedef struct _rwhctx_t{
    const char   *prompt;        /* prompt */
    ringbuf_t    *history;       /* history of lines enterd in the console */
    completion_t *candidate;     /* search target at completion */
    char         *sc_head;       /* shortcut for go to the head of the line */
    char         *sc_tail;       /* shortcut for go to the tail of the line */
    char         *sc_next_block; /* shortcut for go to the next edge of the word of the line */
    char         *sc_prev_block; /* shortcut for go to the previous edge of the word of the line */
    char         *sc_completion; /* shortcut for completion */
    char         *sc_dive_hist;  /* shortcut for fetch older history */
    char         *sc_float_hist; /* shortcut for fetch newer history */
}rwhctx_t;

extern completion_t* /* NULL if fails */
genCompletion( /* generate a completion_t */
        const char **strings,      /* [in] search target at completion */
        int          string_num);  /* number of candidate */

extern rwhctx_t* /* a generated rwh_ctx_t pointer which shortcut setting fields are set to default. if failed, it will be NULL. */
genRwhCtx( /* generate a rwh_ctx_t pointer. */
        const char  *prompt,         /* [in] prompt */
              int    history_size,   /* max size of the buffer of the history */
        const char **candidates,     /* [in] search target at completion */ 
              int    candidate_num); /* number of candidates */

extern char * /* enterd line */
rwh( /* acquire the line entered in the console and keep history. */
        rwhctx_t   *ctx);      /* [mod] an context generated by genRwhCtx(). ctx keeps shortcuts and history operation keys settings and history. after rwh(), the entories of history of ctx is updated. */

extern void
freeRwhCtx( /* free rwhctx_t pointer recursively. */
        rwhctx_t *ctx); /* [mod] to be freed */

#endif 
