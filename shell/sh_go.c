/**
 *  @file   sh_rx.c
 *  @brief  loadx command
 *  $Id: sh_go.c,v 1.1.1.1 2013/12/18 03:43:35 jedy Exp $
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
#include <mipsregs.h>
#include <serial.h>
#include <cache.h>

command_init (sh_go, "go", "go <addr>");

__mips32__
void go32(char *addr)
{
	char *entry;
	int (*go_main)(int argc, char *argv, char *envp, void *null);    
	
	entry = addr;
    go_main = (void *)entry;
    flush_cache_all();    
    go_main(1, NULL, NULL, NULL);    
}

static int sh_go (int argc, char **argv, void **result, void *sys_ctx)
{
    char *addr;
    
    int rc;

	if (argc < 2) {        
        goto EXIT;
    }

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;    
    
    go32(addr);
    
    return 0;

EXIT:
    print_usage(sh_go);
    return -1;
}

