#ifndef READLINE_H
#define READLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_HISTORY_ENTRY_NUM 32
#define MYCONIO_MAX_LINE_SIZE 256

extern char getch(void);

typedef enum _eREADLINE_WITH_HISTORY_RET{
    eREADLINE_WITH_HISTORY_SUCCESS,
}eREADLINE_WITH_HISTORY_RET;

extern eREADLINE_WITH_HISTORY_RET
readlineWithHistory( /* acquire the line entered in the console and keep history by libreadline.a */
        const char *prompt,  /* [in]  string to be output to the console when urging the usr input */
              char *line);   /* [out] obtained line */

#endif 
