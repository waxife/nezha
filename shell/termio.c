/**
 *  @file   termio.c
 *  @brief  terminal IO
 *  $Id: termio.c,v 1.6 2014/02/13 11:53:20 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.6 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <vt100.h>
#include <string.h>
#include <serial.h>
#include <shell.h>
#include <sd.h>
#include <watchdog.h>

#define HISTORY_CNT             2
#define MAX_HIST_STRING_SIZE    64

const int history_cnt = HISTORY_CNT;

extern int puts0 (char *buf);

static inline int decode0(char ch)
{
    switch(ch) {
    case 'A':   return CURSOR_UP;
    case 'B':   return CURSOR_DOWN;
    case 'C':   return CURSOR_RIGHT;
    case 'D':   return CURSOR_LEFT;
    case 'H':   return HOME;
    case 'K':   return END;
    }

    return 0;
}

static inline int decode1(char ch)
{
    switch(ch) {
    case 0x31:  return HOME;
    case 0x32:  return INSERT;
    case 0x33:  return DELETE;
    case 0x34:  return END;
    case 0x35:  return PAGE_UP;
    case 0x36:  return PAGE_DOWN;
    }

    return 0;
}

extern int idle_job(void);

static int getkey(void)
{
    int ch, key = 0, pos=0, c1 = 0;
    static char sd_inserted = -1;
    int rc;
    
    while(1) {
        while((ch = getb2()) < 0) {
            /* read CPU timer to refresh timer */
            read_cpu_count();

            /* detect SD card is inserted or not */
            rc = sdc_carddetected();
            if (rc < 0) {
                if (sd_inserted != 0) {
                    printf("\nSD is removed.\n");
                    sd_inserted = 0;
                    return ETX;
                }
            } else {
                if (sd_inserted != 1) {
                    printf("\nSD is inserted.\n");
                    sd_inserted = 1;
                    return ETX;
                }
            }

            if (pos == 0) {
                rc = idle_job();
                if (rc != 0)
                    return -1;
            }

            /* touch for keep alive */
            watchdog_touch();
        }
    
        switch(pos) {
        case 0: /* ? */
            if (ch != ESC)
                return ch;
            pos++;
            break;
        case 1: /* ESC ? */
            if (ch != BKT) 
                return ch;
            pos++;
            break;
            
        case 2: /* ESC [ ?*/
            key = decode0(ch);
            if (key)
                return key;
            c1 = ch;
            pos++;
            break;
        case 3: /* ESC [ c1 ? */
            if (ch == WAV) {
                key = decode1(c1);
                return key;
            }
            return ch;
        }
    }

    return 0;
}

#if 0
static inline void cr_cursor(void)
{
    puts0("\x1b[u");   /* restore cursor position */
}
#endif

static inline void new_prompt(void)
{
    puts0("\x1b[M");   /* delete line */
    puts0("\x1b[2K");  /* erase line */
    puts0("\x1b[128D");  /* move cursor to begin of line */
    puts0("Nezha> ");
}

static inline void cur_backward(int cnt)
{
    printf("\x1b[%dD", cnt);    /* backward cursor */
}

static inline void cur_forward(int cnt)
{
    printf("\x1b[%dC",cnt);     /* forward cursor */
}

static inline void del_character(void)
{
    puts0("\x1b[P");           /* delete a character */
}



int term = T_VT102;

static inline void ins_character(int ch, char *pos, int size)
{
    int i;

    switch(term) {
    case T_VT102:
        puts0("\x1b[@");           /* insert character */
        putchar(ch);
        break;

    case T_VT100:
    default: 
        if (size == 0) {
            putchar(ch);
        } else {
            putchar(ch);
            puts0("\x1b[s");
            puts0("\x1b[0J");
            for (i = 0; i < size; i++)
                putchar(*pos++);
            puts0("\x1b[u");
        }
        break;
    }
}

char hist_command[HISTORY_CNT][MAX_HIST_STRING_SIZE];
int  history = 0;


/*
 *   |<-        MAX_HIST_STRING_SIZE       ->|
 *
 *   |---------+---------+---------+---------|
 *   V         V         V         V         V
 * string     cut       pos       eol       end
 * 
 *             |<-    visible    ->|
 */

char *get_str(void)
{
    char *string, *cut, *pos, *eol, *end;
    int hcur = history % HISTORY_CNT;
    int hidx = hcur;
    int ch;
    int len;

    string = hist_command[hcur];
    cut = pos = eol = string;
    end = string + MAX_HIST_STRING_SIZE - 1;
    *eol = '\0';

    new_prompt();

    while(1) {
        ch = getkey();
        
        if (ch >= 0x20 && ch <= 0x7e && eol < end) {
            /* insert a character */
            ins_character(ch, pos, eol-pos);
            memcpy(pos+1, pos, eol-pos);
            *pos = ch;
            pos++;
            eol++;
        } else if (ch == ETX) { /* Ctrl-C  */
            /* Abort */
            putchar(NL);
            cut=pos=eol=string;
            new_prompt();
        } else if (ch == CR) {  /* Enter */
            putchar(NL);
            if (*(pos-1) == '\\' && pos == eol) { /* check continue '\' */
                /* Continue */
                puts0("> ");
                *(pos - 1) = ' ';
                cut = pos;
            } else {    
                /* End of Line */
                len = eol - string;
                if (string != hist_command[history%HISTORY_CNT])
                    memcpy(hist_command[history%HISTORY_CNT], string, len+1);
                if (len)
                    history++;
                return string;
            }
        } else if (ch == BS && pos > cut) {
            /* backspace */
            cur_backward(1);
            del_character();
            memcpy(pos - 1, pos, eol - pos);
            pos = pos - 1;
            eol = eol - 1;
        } else if (ch == DELETE && pos < eol) {
            /* delete */
            del_character();
            memcpy((pos+1) - 1, (pos+1), eol - (pos + 1));
            eol = eol - 1;
        } else if (ch == HOME) {
            /* move cursor to the head of line */
            cur_backward(pos - cut);
            pos = cut;
        } else if (ch == END) {
            /* move cursor to the end of line */
            cur_forward(eol - pos);
            pos = eol;
        } else if (ch == CURSOR_RIGHT && pos < eol) {
            /* move cursor to the right */
            cur_forward(1);
            pos++;
        } else if (ch == CURSOR_LEFT && pos > cut ) {
            /* move cursor to the left */
            cur_backward(1);
            pos--;
        } else if (ch == CURSOR_UP && history > 0) {
            /* previous command */
            cur_backward(pos - cut);
            if (hidx == 0 && history < HISTORY_CNT) {
                hidx = history;
            } else {
                hidx = (hidx + HISTORY_CNT - 1) % HISTORY_CNT;
            }
            strcpy(hist_command[hcur], hist_command[hidx]);
            string = cut = hist_command[hcur];
            pos = eol = string + strlen(string);
            end = string + MAX_HIST_STRING_SIZE - 1;
            //cr_cursor();
            new_prompt();
            puts0(string);
        } else if (ch == CURSOR_DOWN) {
            /* new command */
            cur_backward(pos - cut);
            if (hidx == history) {
                hidx = 0;
            } else {
                hidx = (hidx + 1) % HISTORY_CNT;
            }
            strcpy(hist_command[hcur], hist_command[hidx]);
            string = cut = hist_command[hcur];
            pos = eol = string + strlen(string);
            end = string + MAX_HIST_STRING_SIZE - 1;
            //cr_cursor();
            new_prompt();
            puts0(string);
        } else if (ch == PAGE_UP) {
            /* first command in the history list */
            cur_backward(pos - cut);
            if (history < HISTORY_CNT) {
                hidx = 0;
            } else {
                hidx = (history+1) % HISTORY_CNT;
            }
            strcpy(hist_command[hcur], hist_command[hidx]);
            string = cut = hist_command[hcur];
            pos = eol = string + strlen(string);
            end = string + MAX_HIST_STRING_SIZE - 1;
            puts0("\x1b[u");   /* save cursor position */
            //cr_cursor();
            new_prompt();
            puts0(string);
        } else if (ch == PAGE_DOWN) {
            /* last command in the history list */
            cur_backward(pos - cut);
            hidx = history  % HISTORY_CNT;
            strcpy(hist_command[hcur], hist_command[hidx]);

            string = cut = hist_command[hcur];
            pos = eol = string + strlen(string);
            end = string + MAX_HIST_STRING_SIZE - 1;
            //cr_cursor();
            new_prompt();
            puts0(string);
        } else if (ch < 0) {
            /* refresh command */
            new_prompt();
            puts0(string);
        }
        
        *eol = '\0';

    }
}


