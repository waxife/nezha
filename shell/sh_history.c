/**
 *  @file   sh_history.c
 *  @brief  shell "script" command
 *  $Id: sh_history.c,v 1.2 2014/01/27 02:27:48 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <serial.h>

command_init (sh_history, "history", "history [<num>]");

extern const int history_cnt; 
extern char *hist_command[];
extern int  history;


static int sh_history (int argc, char **argv, void **result, void *sys_ctx)
{
    int num;
    int hidx;
    int i;
    
    if (argc > 1) {
        num = atoi(argv[1]);
        hidx = history - history_cnt;
        if (hidx < 0)
            hidx = 0;
        if (num <= history && num >= hidx) {
            printf("%d %s\n", num, hist_command[num % history_cnt]);
        } else {
            printf("history %d ~ %d\n", hidx, history);
        }
    } else {
        hidx = history - history_cnt;
        if (hidx < 0)
            hidx = 0;

        for (i = hidx; i < history; i++) {
            printf("%d %s\n", i, hist_command[i % history_cnt]);
        }

    }

    return 0;
}
