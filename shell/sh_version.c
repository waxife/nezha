/**
 *  @file   sh_version.c
 *  @brief  system init entry
 *  $Id: sh_version.c,v 1.2 2014/01/27 02:27:48 jedy Exp $
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

command_init (sh_version, "version", "version");

extern int __NEZHA_VER;
extern int __BUILD_NUMBER;
extern int __BUILD_DATE;
static int sh_version (int argc, char **argv, void **result, void *sys_ctx)
{
    printf("Nezha v%d.%02d\n", (unsigned)&__NEZHA_VER/100, (unsigned)&__NEZHA_VER%100);
    printf("Build %d-%d\n", (unsigned)&__BUILD_DATE, (unsigned)&__BUILD_NUMBER);
    printf("Copyright 201333 Terawins Inc.\n\n");

    return 0;
}

