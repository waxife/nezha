/**
 *  @file   sh_reg_config.c
 *  @brief  configure registry for nor flash
 *  $Id: 
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/30  dora    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <cache.h>
#include <ctype.h>
#include <unistd.h>
#include <debug.h>
#include <io.h>
#include <gpio.h>
//#include <fis_struct.h>
#include <registry.h>

command_init (sh_reg_dump, "reg_dump", "reg_dump");

static int sh_reg_dump(int argc, char **argv, void **result, void *sys_ctx)
{   
    int i, nitems, rc;
    char name[4];
    void *val;

    if (is_configured() < 0){
        printf("Registry is not configured !!! \n");
        printf("Please use reg_config command to configure it first. \n");
        goto EXIT;
    }
    
    if (argc > 1) 
        goto EXIT;
    
    rc = reg_items(&nitems);
    if (rc < 0)
        goto EXIT;
    
    i = 0;
    printf("Name              Value       \n");
    printf("----------------  ----------\n");
    for (i = 0; i < nitems; i++) {
        rc = reg_idx_get(i, name, &val);
        if (rc < 0)
            goto EXIT;
        printf("%-16s  %d \n", name, (int)val);
    }
        
    
    return 0;

EXIT:
    print_usage (sh_reg_dump);
    return -1;
}
