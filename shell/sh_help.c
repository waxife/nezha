/**
 *  @file   sh_help.c
 *  @brief  shell "help" command
 *  $Id: sh_help.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
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

command_init (sh_help, "help", "help");

static int sh_help (int argc, char **argv, void **result, void *sys_ctx)
{
	cmd_t *cmd;
    int show = 0;
    char *synopsis;
    int internal;
    int all = 0;

    if (argc > 1 && strcmp(argv[1], "all") == 0) {
        all = 1;
    }

    if (argc > 1 && strcmp(argv[1], "internal") == 0) {
        show = 1;
    }

	for (cmd = &__cmd_start; cmd < &__cmd_end; cmd++) {
        if (cmd->synopsis) {
            synopsis = cmd->synopsis;
            if (*synopsis == '-') {
                internal = 1;
                synopsis++;
            } else {
                internal = 0;
            }

            if (all || (show == internal)) {
	    	    printf ("%-16s - %s\n", cmd->name, synopsis);
            }
        } else {
            printf("%-16s - \n", cmd->name);
        }
    }

	return 0;
}
