/**
 *  @file   sh_vt.c
 *  @brief  system init entry
 *  $Id: sh_vt.c,v 1.2 2014/01/27 12:26:24 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/01  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <cache.h>

command_init (sh_vt102, "vt100", "vt100");
command_alias (sh_vt102, 0, "vt102", "vt102");
command_alias (sh_vt102, 1, "ansi", "ansi");

extern int term;

static int sh_vt102 (int argc, char **argv, void **result, void *sys_ctx)
{
    if (strcmp(argv[0], "vt100") == 0) 
        term  = T_VT100;
    else if (strcmp(argv[0], "vt102") == 0)
        term = T_VT102;
    else if (strcmp(argv[0], "ansi") == 0)
        term = T_ANSI;

    return 0;
}
