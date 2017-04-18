/**
 *  @file   sh_i2cget.c
 *  @brief  i2cget, copy file to memory through i2c path
 *  $Id: sh_i2cget.c,v 1.1.1.1 2013/12/18 03:43:40 jedy Exp $
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
#include <sys.h>
#include <io.h>
#include "i2cget.h"


command_init (sh_i2cget, "i2cget", "i2cget <addr>");

int sh_i2cget(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0;
    char* filename = "0x80200000";

    if(argc >= 2)
        filename = argv[1];
    else
        goto EXIT;

    printf("i2cget, filename = %s\n", filename);
    rc = i2cget(filename);
    if(rc !=0 )
    {
        printf("error no = %d\n", rc);
            return -1;
    }
    printf("write data to %s success\n", filename);
    return 0;
EXIT:
    print_usage(sh_i2cget);
    return -1;
}

