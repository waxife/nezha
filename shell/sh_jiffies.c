/**
 *  @file   sh_sysinfo.c
 *  @brief  system init entry
 *  $Id: sh_jiffies.c,v 1.1.1.1 2013/12/18 03:43:37 jedy Exp $
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
#include <mipsregs.h>
#include <sys.h>

command_init (sh_jiffies, "jiffies", "jiffies");

static int sh_jiffies (int argc, char **argv, void **result, void *sys_ctx)
{
    extern unsigned int o_jiffies;
    
    printf("%u %u\n", jiffies, o_jiffies);

    return 0;
}
