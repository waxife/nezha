/**
 *  @file   sh_script.c
 *  @brief  shell "script" command
 *  $Id: sh_script.c,v 1.2 2014/01/27 12:26:24 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <serial.h>

command_init (sh_script, "script", "script <addr>");
command_alias(sh_script, 0, "s", "script <addr>");

extern int rx(char *);

static int sh_script (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    int len = 0;
    unsigned long terminate = 0x1a000000;

	if (argc < 2) {
		print_usage (sh_script);
		return -1;
	}

	rc = get_addr(argv[1], &addr);
	if ( rc<0 )
	    goto EXIT;

    printf("Ready to receive xmodem data to %p\n", addr);
    len = rx(addr);
    if ( len<0 ) {
        printf("Error occured or user abandon\n");
    } else {
        printf("\naddr: %p\nSize: %xh (%d) received\n",  addr, len, len);
    }

    memcpy(addr+len, &terminate, sizeof(int));

    if (*addr != '#') {
        printf("%s is not a valid script\n", argv[1]);
        printf("The first line of script must be a comment line. For example\n");
        printf("# !/tboot\n");
        goto EXIT;
    }
    
    printf("\nTo execute script...\n\n");

    *result = (void *)addr;

    return 0;

EXIT:
	return -1;
}
