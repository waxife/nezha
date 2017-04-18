/**
 *  @file   sh_dump_param.c
 *  @brief  dump parameters in nor code (offset 0x10~0x100)
 *  $Id: sh_dump_param.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/13  yc    New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <unistd.h>
#include <stdlib.h>

#define NOR_PARAM 0xa0000010

command_init(sh_dump_param, "dump_param", "dump parameters in Nor code");

int sh_dump_param(int argc, char **argv, void **result, void *sys_ctx)
{    
    if(argc >= 2)
        goto EXIT;

    printf("Dump Parameters\n");
    printf(">>>>>\n");
    printf("%s", (char*) NOR_PARAM);
    printf("<<<<<\n");

    return 0;

EXIT:

    print_usage(sh_dump_param);
    return -1;
}


