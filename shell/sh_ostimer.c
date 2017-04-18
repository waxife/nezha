/**
 *  @file   sh_sysinfo.c
 *  @brief  system init entry
 *  $Id: sh_ostimer.c,v 1.1.1.1 2013/12/18 03:43:40 jedy Exp $
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

command_init (sh_ostimer, "ostimer", "ostimer");

static int sh_ostimer (int argc, char **argv, void **result, void *sys_ctx)
{
    extern int t300_ostimer_init(void);
    int rc;
    
    printf("init ostimer... \n");
    rc = t300_ostimer_init();
    if ( rc < 0 ) {
        printf("failed, rc %d\n", rc);
        return rc;
    }
    
    printf("ok\n");
    
    return 0;
}
