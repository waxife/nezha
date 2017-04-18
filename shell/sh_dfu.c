/**
 *  @file   sh_dfu.c
 *  @brief  command for jump to IROM dfu mode
 *  $Id: sh_dfu.c,v 1.2 2014/02/13 11:53:20 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/12/15  sherman    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys.h>
#include <cache.h>
#include <watchdog.h>

command_init (sh_dfu, "dfu", "dfu ");

#define DFU_ENTRY		0xbfc00b98	// for FPGA
//#define	DFU_ENTRY		0xbfc00bbc	// for ASIC

static int sh_dfu (int argc, char **argv, void **result, void *sys_ctx)
{
    char *entry = NULL;
    int (*go_main)(int argc, char *argv, char *envp, void *null);

    if (argc > 1)
    	goto EXIT;

    watchdog_disable();
    	   	
	entry = (char *)DFU_ENTRY;
	printf("entry 0x%x \n", (unsigned int)entry);

    go_main= (void *)entry;
    flush_cache_all();
    go_main(1, NULL, NULL, NULL);
    
    return 0;

EXIT:
    print_usage(sh_dfu);
    return -1;
}


