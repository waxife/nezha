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
#include <registry.h>

command_init (sh_reg_put, "reg_put", "reg_put <name> <val>");

static int sh_reg_put(int argc, char **argv, void **result, void *sys_ctx)
{   
    int rc;
    char *pname;
    void *pval = NULL;
 
    if (is_configured() < 0){
        printf("Registry is not configured !!! \n");
        printf("Please use reg_config command to configure it first. \n");
        goto EXIT;
    }
    
    if (argc < 3) 
        goto EXIT;
        
    pname = argv[1];
    pval = argv[2];
    
    printf("name %s pval %s \n", pname, (char *)pval);
    
    rc = reg_put(pname, (int *)strtol(argv[2], NULL, 0));
    if (rc < 0) {
        dbg(2, "registry put failed, rc %d \n", rc);
        goto EXIT;
    }
    
    printf("Registry put successfully \n");
    
    return 0;

EXIT:
    print_usage (sh_reg_put);
    return -1;
}
