/**
 *  @file   sh_reg_config.c
 *  @brief  configure registry for nand or nor flash
 *  $Id: 
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/30    dora    New file.
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
#include <serial.h>
#include <nor.h>

command_init (sh_reg_config, "reg_config", "reg_config <ba>");

static int sh_reg_config (int argc, char **argv, void **result, void *sys_ctx)
{
    struct sys_ctx_t *sys = (struct sys_ctx_t *)sys_ctx;
    struct norc_t *norc = sys->pnor;
    void *conf = NULL;  
    int rc;
    int ba, pa;
       
    if (argc < 2) 
        goto EXIT;

    ba = strtol(argv[1], NULL, 0);
    
    printf("Start configure registry ... \n");
    
        
    pa = ba2pa(norc, ba);
    if (pa < 0)
        goto EXIT;
        
    conf = (void *)reg_config_nor(pa); 
    
    rc = reg_init(conf);
    if (rc < 0)
        goto EXIT;


    return 0;

EXIT:
    print_usage(sh_reg_config);
    return -1;
}
