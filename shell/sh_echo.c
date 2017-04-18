/**
 *  @file   sh_echo.c
 *  @brief  system init entry
 *  $Id: sh_echo.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
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

command_init (sh_echo, "echo", "echo <str>");


static int sh_echo (int argc, char **argv, void **result, void *sys_ctx)
{
    int i;
    for (i = 1; i < argc; i++)
        printf("%s ", argv[i]);

    printf("\n");

    return 0;
}
