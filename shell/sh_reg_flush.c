/**
 *  @file   sh_reg_flush.c
 *  @brief  flush shadow to nor flash
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
#include <registry.h>

command_init (sh_reg_flush, "reg_flush", "reg_flush");

static int sh_reg_flush(int argc, char **argv, void **result, void *sys_ctx)
{   
    int rc;
    
    if (is_configured() < 0){
        printf("Registry is not configured !!! \n");
        printf("Please use reg_config command to configure it first. \n");
        goto EXIT;
    }
    
    if (argc > 1) 
        goto EXIT;
    
    rc = reg_flush();
    if (rc < 0) {
         //retry again 
         rc = reg_flush();
         if (rc < 0)            
            goto EXIT;
    }
   
    
    return 0;

EXIT:
    print_usage (sh_reg_flush);
    return -1;
}
