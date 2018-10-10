#include "prompt.h"

const char DEFAULT_SC_HEAD[]       = {0x01};
const char DEFAULT_SC_TAIL[]       = {0x05};
const char DEFAULT_SC_NEXT_BLOCK[] = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x44};
const char DEFAULT_SC_PREV_BLOCK[] = {0x1b, 0x5b, 0x31, 0x3b, 0x35, 0x43};
const char DEFAULT_SC_COMPLETION[] = {0x09};
const char DEFAULT_SC_DIVE_HIST[]  = {0x1b, 0x5b, 0x41};
const char DEFAULT_SC_FLOAT_HIST[] = {0x1b, 0x5b, 0x42};

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
    ctx -> entry_max = history_size;
    if(!(ctx -> history       = (char **)calloc(history_size, sizeof(char *)))){
        free(ctx);
        return NULL;
    }
    memset(ctx -> history, 0x00, history_size);
    ctx -> sc_head       = malloc(sizeof(char)*strlen(DEFAULT_SC_HEAD));
    ctx -> sc_tail       = malloc(sizeof(char)*strlen(DEFAULT_SC_TAIL));
    ctx -> sc_next_block = malloc(sizeof(char)*strlen(DEFAULT_SC_NEXT_BLOCK));
    ctx -> sc_prev_block = malloc(sizeof(char)*strlen(DEFAULT_SC_PREV_BLOCK));
    ctx -> sc_completion = malloc(sizeof(char)*strlen(DEFAULT_SC_COMPLETION));
    ctx -> sc_dive_hist  = malloc(sizeof(char)*strlen(DEFAULT_SC_DIVE_HIST));
    ctx -> sc_float_hist = malloc(sizeof(char)*strlen(DEFAULT_SC_FLOAT_HIST));
    strcpy(ctx -> sc_head,       DEFAULT_SC_HEAD);
    strcpy(ctx -> sc_tail,       DEFAULT_SC_TAIL);
    strcpy(ctx -> sc_next_block, DEFAULT_SC_NEXT_BLOCK);
    strcpy(ctx -> sc_prev_block, DEFAULT_SC_PREV_BLOCK);
    strcpy(ctx -> sc_completion, DEFAULT_SC_COMPLETION);
    strcpy(ctx -> sc_dive_hist,  DEFAULT_SC_DIVE_HIST);
    strcpy(ctx -> sc_float_hist, DEFAULT_SC_FLOAT_HIST);
    return ctx;
}

void
freeRwhCtx(sRwhCtx *ctx){
    for(int i=0; ctx->history[i]!=NULL; i++){
        free(ctx -> history[i]);
    }
    free(ctx -> history);
}

char *
rwh(
        sRwhCtx    *ctx,
        const char *prompt) /* in */
{
    char *line;
    line = readline(prompt);
    return line;

/*     const int MAX_HISTORY_ENTRY_NUM = 30; [> tmp <]
 *     char ch;
 *     char buff[30] = {0}; [> 30 is tmp <]
 *     int  entry_idx = g_hist_entry_size;
 *     char current_buff_is_historied = 0;
 *     int  cursor_idx = 0;
 *     char *line = malloc(256);
 * 
 *     printf("%s", prompt);
 * 
 *     while(1){
 *         ch = getch();
 * 
 *         switch(ch){
 *             case 0x1b: [> 方向キー <]
 *                 if(getch() == 0x5b){
 *                     switch(getch()){
 *                         case 0x41: [>↑<]
 *                             [> 途中まで入力してた内容をhistoryに入れる <]
 *                             if(!current_buff_is_historied && g_hist_entry_size > 0){
 *                                 add_history(buff);
 *                                 g_hist_entry_size = g_hist_entry_size == MAX_HISTORY_ENTRY_NUM ? MAX_HISTORY_ENTRY_NUM : g_hist_entry_size + 1;
 *                                 current_buff_is_historied = 1;
 *                                 memset(buff, '\0', sizeof(buff));
 *                             }
 * 
 *                             [> 見える部分の処理 <]
 *                             entry_idx--;
 *                             if(entry_idx < 0){
 *                                 entry_idx = 0;
 *                                 break;
 *                             }
 *                             for(int i=0;i<cursor_idx;i++){
 *                                 printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                             }
 *                             for(int i=0;i<strlen(buff);i++){
 *                                 printf(" ");
 *                             }
 *                             for(int i=0;i<strlen(buff);i++){
 *                                 printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                             }
 *                             {
 *                                 [> 見える部分の処理 <]
 *                                 HIST_ENTRY **hist_list = history_list();
 *                                 printf("%s", hist_list[entry_idx] -> line);
 * 
 *                                 [> buffの処理 <]
 *                                 strcpy(buff, hist_list[entry_idx] -> line);
 *                                 cursor_idx = strlen(buff);
 *                             }
 *                             break;
 * 
 *                         case 0x42: [>↓<]
 *                             [> 見える部分の処理 <]
 *                             entry_idx++;
 *                             if(entry_idx > g_hist_entry_size - 1){
 *                                 entry_idx = g_hist_entry_size - 1;
 *                                 break;
 *                             }
 *                             for(int i=0;i<cursor_idx;i++){
 *                                 printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                             }
 *                             for(int i=0;i<strlen(buff);i++){
 *                                 printf(" ");
 *                             }
 *                             for(int i=0;i<strlen(buff);i++){
 *                                 printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                             }
 *                             {
 *                                 [> 見える部分の処理 <]
 *                                 HIST_ENTRY **hist_list = history_list();
 *                                 printf("%s", hist_list[entry_idx] -> line);
 * 
 *                                 [> buffの処理 <]
 *                                 strcpy(buff, hist_list[entry_idx] -> line);
 *                                 cursor_idx = strlen(buff);
 *                             }
 *                             break;
 * 
 *                         case 0x43: [> → <]
 *                             if(cursor_idx == strlen(buff)){
 *                                 break;
 *                             }
 *                             printf("%c%c%c", 0x1b, 0x5b, 0x43);
 *                             cursor_idx++;
 *                             break;
 * 
 *                         case 0x44: [>←<]
 *                             if(cursor_idx == 0){
 *                                 break;
 *                             }
 *                             printf("%c%c%c", 0x1b, 0x5b, 0x44);
 *                             cursor_idx--;
 *                             break;
 * 
 *                         default:
 *                             break;
 *                     }
 *                 }
 *                 break;
 * 
 *             case 0x1: [>Ctr-A<]
 *                 for(int i=0;i<cursor_idx;i++){
 *                     printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                 }
 *                 cursor_idx = 0;
 *                 break;
 * 
 *             case 0x5: [>Ctr-E<]
 *                 for(int i=0;i<strlen(buff)-cursor_idx;i++){
 *                     printf("%c%c%c", 0x1b, 0x5b, 0x43);
 *                 }
 *                 cursor_idx = strlen(buff);
 *                 break;
 * 
 *             case 0x7f: [>backspace<]
 *                 if(cursor_idx == 0){
 *                     break;
 *                 }
 * 
 *                 printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                 for(int i=0;i<strlen(&buff[cursor_idx])+1;i++){
 *                     printf(" "); 
 *                 }
 *                 for(int i=0;i<strlen(&buff[cursor_idx])+1;i++){
 *                     printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                 }
 *                 printf("%s", &buff[cursor_idx]);
 *                 for(int i=0;i<strlen(&buff[cursor_idx]);i++){
 *                     printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                 }
 * 
 *                 sprintf(&buff[cursor_idx-1], "%s", &buff[cursor_idx]);
 *                 cursor_idx--;
 *                 break;
 * 
 *             case '\n': 
 *                 if(current_buff_is_historied){
 *                     remove_history(g_hist_entry_size - 1);
 *                     g_hist_entry_size--;
 *                 }
 *                 putchar('\n');
 *                 add_history(buff);
 *                 g_hist_entry_size = g_hist_entry_size == MAX_HISTORY_ENTRY_NUM ? MAX_HISTORY_ENTRY_NUM : g_hist_entry_size + 1;
 *                 strcpy(line, buff);
 *                 return line;
 * 
 *             default:
 *                 for(int i=0;i<strlen(&buff[cursor_idx]);i++){
 *                     printf("%c%c%c%c", 0x1b, 0x5b, 0x33, 0x7e); [> delete <]
 *                 }
 *                 putchar(ch);
 *                 printf("%s", &buff[cursor_idx]);
 *                 for(int i=0;i<strlen(&buff[cursor_idx]);i++){
 *                     printf("%c%c%c", 0x1b, 0x5b, 0x44); [> ← <]
 *                 }
 *                 {
 *                     char tmp_buff[1000] = {'\0'};
 *                     strcpy(tmp_buff, &buff[cursor_idx]);
 *                     sprintf(&buff[cursor_idx++], "%c%s", ch, tmp_buff);
 *                 }
 *                 break;
 *         }
 *     }
 * 
 *     return line; */
}
