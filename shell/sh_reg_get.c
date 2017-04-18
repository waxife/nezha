/**
 *  @file   sh_reg_get.c
 *  @brief  get registry value
 *  $Id: 
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/30  dora   New file.
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

command_init (sh_reg_get, "reg_get", "reg_get <name>");

static int sh_reg_get(int argc, char **argv, void **result, void *sys_ctx)
{   
    //struct sys_ctx_t *sys = (struct sys_ctx_t *)sys_ctx;
    int rc;
    char *name;
    void *val;
    
    if (is_configured() < 0){
        printf("Registry is not configured !!! \n");
        printf("Please use reg_config command to configure it first. \n");
        goto EXIT;
    }
    
    val = NULL;
    if (argc < 2) 
        goto EXIT;
    
    name = argv[1];
    
    printf("Name              Value       \n");
    printf("----------------  ----------\n");
    
    rc = reg_get(name, &val);    
    if (rc < 0)    
        goto EXIT;

    
    printf("%-16s  %d \n", name, (int)val);
    
    
    return 0;

EXIT:
    print_usage(sh_reg_get);
    return -1;
}
